#include "VehicleDummy.h"

std::vector<RwFrame*> VehicleDummy::m_Frames;
RwMatrix* tempMat = NULL;

void VehicleDummy::FindDummies(CVehicle* vehicle, RwFrame* frame) {
	if (!frame)
		return;

	if (RwFrame* nextFrame = frame->child)
		FindDummies(vehicle, nextFrame);

	if (RwFrame* nextFrame = frame->next)
		FindDummies(vehicle, nextFrame);

	if (std::find(m_Frames.begin(), m_Frames.end(), frame) != m_Frames.end())
		return;

	m_Frames.push_back(frame);
}

void VehicleDummy::CreateTempMatrix()
{
	if (!tempMat) tempMat = RwMatrixCreate();
}

std::vector<RwFrame*> VehicleDummy::GetFramesOnVehicle(CVehicle* vehicle) {
	m_Frames.clear();
	FindDummies(vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);
	return m_Frames;
}

std::vector<RwFrame*> VehicleDummy::GetFrameHierarchy(RwFrame* frame, RwFrame* root) {
	std::vector<RwFrame*> hie;
	RwFrame* f = frame;

	while (f != NULL && f != root)
	{
		hie.insert(hie.begin(), f);
		f = RwFrameGetParent(f);
	}
	return hie;
}

/*
Takes the vehicle world position and adds offset
*/
CVector VehicleDummy::GetTransformedPosition(CVehicle* vehicle, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;
	auto c = rootFrame->child;

	//Log::file << "child= " << GetFrameNodeName(c) << std::endl;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	if (ToLower(GetFrameNodeName(c)).find("chassis_dummy") != -1)
	{
		RwMatrixTransform(tempMat, RwFrameGetMatrix(c), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

/*
Takes the world position of a dummy and adds offset from its position
*/
CVector VehicleDummy::GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		RwMatrixTransform(tempMat, RwFrameGetMatrix(hf), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

/*
Takes the world position of a dummy (by name) and adds offset from its position
*/
CVector VehicleDummy::GetTransformedDummyPositionByName(CVehicle* vehicle, std::string dummyName, CVector offset) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	auto frame = FindDummy(vehicle, dummyName);

	if (!frame) return CVector(0, 0, 0);

	return VehicleDummy::GetTransformedDummyPosition(vehicle, frame, offset);
}

RwFrame* VehicleDummy::FindDummy(CVehicle* vehicle, std::string dummyName) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	for (auto frame : frames)
	{
		std::string frameName = ToLower(GetFrameNodeName(frame));

		if (frameName.compare(ToLower(dummyName)) == 0) {
			return frame;
		}
	}

	return NULL;
}

CVector VehicleDummy::GetDummyOffset(CVehicle* vehicle, std::string dummyName) {
	CVector offset = CVector(0, 0, 0);
	auto dummy = FindDummy(vehicle, dummyName);

	if (dummy == NULL) return offset;

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {

		auto pos = GetFrameNodePosition(hf);

		offset += pos;
	}

	return offset;
}