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

CVector VehicleDummy::GetTransformedPosition(CVehicle* vehicle, CVector position) {
	if (!tempMat) tempMat = RwMatrixCreate();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;
	auto c = rootFrame->child;

	//Log::file << "child= " << GetFrameNodeName(c) << std::endl;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	if (ToLower(GetFrameNodeName(c)).find("chassis_dummy") != -1)
	{
		RwMatrixTransform(tempMat, RwFrameGetMatrix(c), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &position.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

CVector VehicleDummy::GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	if (!tempMat) tempMat = RwMatrixCreate();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		RwMatrixTransform(tempMat, RwFrameGetMatrix(hf), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}

CVector VehicleDummy::FindTransformedDummyPosition(CVehicle* vehicle, std::string dummyName, CVector offset) {
	auto frames = VehicleDummy::GetFramesOnVehicle(vehicle);

	for (auto frame : frames)
	{
		std::string frameName = ToLower(GetFrameNodeName(frame));

		if (frameName.compare(ToLower(dummyName)) == 0) {
			return VehicleDummy::GetTransformedDummyPosition(vehicle, frame, offset);
		}
	}

	return CVector(0, 0, 0);
}