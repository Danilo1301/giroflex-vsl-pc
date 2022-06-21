#include "VehicleDummy.h"
#include "CVisibilityPlugins.h"

std::vector<RwFrame*> VehicleDummy::m_Frames;
std::vector<RwFrame*> VehicleDummy::m_CallbackFrames;
RwMatrix* tempMat = NULL;

std::string FrameName(RwFrame* frame) {
	if (!frame) return "NULL_FRAME";
	return GetFrameNodeName(frame);
}

RwFrame* VehicleDummy::Callback(RwFrame* frame, void* data)
{
	auto parent = RwFrameGetParent(frame);

	Log::file << "---------------" << std::endl;
	Log::file << VehicleDummy::GetFrameInfo(frame) + "\nchild=" << VehicleDummy::GetFrameInfo(frame->child) << "\nnext=" << VehicleDummy::GetFrameInfo(frame->next) << "\nparent=" << VehicleDummy::GetFrameInfo(parent) << std::endl;

	if (std::find(m_CallbackFrames.begin(), m_CallbackFrames.end(), frame) == m_CallbackFrames.end())
		m_CallbackFrames.push_back(frame);

	return frame;
}


void VehicleDummy::FindDummies(CAutomobile* vehicle, RwFrame* frame) {
	if (!frame)
		return;


	m_CallbackFrames.clear();
	RwFrameForAllChildren((RwFrame*)vehicle->m_pRwClump->object.parent, Callback, 0);

	auto parent = RwFrameGetParent(frame);

	//hierarchy

	//vehicle->component

	//Log::file << "--------- FindDummies ------" << std::endl;
	//Log::file << GetFrameInfo(frame) + "\nchild=" << GetFrameInfo(frame->child) << "\nnext=" << GetFrameInfo(frame->next) << "\nparent=" << GetFrameInfo(parent) << std::endl;

	/*
	if (FrameName(frame).find("dummy") != -1)
	{
		Log::file << "* its a dummy " << std::endl;


		//vehicle->SetComponentVisibility(frame, 2);

		FindDummies(vehicle, frame->child);
		FindDummies(vehicle, frame->next);

		static std::vector<RwObject*> objects;
		objects.clear();

		RwFrameForAllObjects(frame, [](RwObject* object, void* data) {

			objects.push_back(object);

			//(RwFrame*)(object)

			//RpAtomic* atomic = (RpAtomic*)object;
			//char szAtomicName[16] = { 0 };

			//auto frame = RpAtomicGetFr(atomic);





			return object;
			}, 0);

		auto localObjects = objects;
		for (auto object : localObjects)
		{

			RpAtomic* atomic = (RpAtomic*)object;

			Log::file << "* object:" << FrameName((RwFrame*)&atomic->object.object) << std::endl;



			FindDummies(vehicle, (RwFrame*)&atomic->object.object);
		}

	}
	*/

	std::vector<RwFrame*> searchFrames;

	for (auto f : m_CallbackFrames)
	{
		Log::file << "*found " << GetFrameInfo(f) << std::endl;

		if (std::find(m_Frames.begin(), m_Frames.end(), frame) == m_Frames.end())
		{
			m_Frames.push_back(frame);
			searchFrames.push_back(frame);
		}

	}

	//Log::file << "* searchFrames" << std::endl;

	for (auto f : searchFrames)
	{
		FindDummies(vehicle, f->child);
		FindDummies(vehicle, f->next);
	}




	/*
	auto parent = RwFrameGetParent(frame);

	Log::file << "---------------" << std::endl;

	if (!frame)
		return;

	if (frame->child)
		FindDummies(vehicle, frame->child);

	if (frame->next)
		FindDummies(vehicle, frame->next);

	if (std::find(m_Frames.begin(), m_Frames.end(), frame) != m_Frames.end())
		return;

	Log::file << "* frame added: " << FrameName(frame) << std::endl;

	//RwFrameForAllChildren

	m_Frames.push_back(frame);
	*/
}

std::vector<RwFrame*> VehicleDummy::GetFramesOnVehicle(CVehicle* vehicle) {

	//Log::file << "get frames" << std::endl;


	m_Frames.clear();
	FindDummies((CAutomobile*)vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);


	//FindDummies(vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);
	
	/*
	for (auto frame : m_Frames)
	{
		Log::file << "FOUND " << FrameName(frame) << std::endl;
	}
	*/

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

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);
	RwMatrixTransform(tempMat, RwFrameGetMatrix(c), rwCOMBINEPRECONCAT);
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
		std::string frameName = GetFrameNodeName(frame);

		if (frameName.compare(dummyName) == 0) {
			return VehicleDummy::GetTransformedDummyPosition(vehicle, frame, offset);
		}
	}

	return CVector(0, 0, 0);
}

std::string VehicleDummy::GetFrameInfo(RwFrame* frame) {
	if (!frame) return "NULL_FRAME";

	auto position = GetFrameNodePosition(frame);
	auto parent = RwFrameGetParent(frame);

	return "[" + FrameName(frame) + ", " + FormatCVector(position) + ", child=" + FrameName(frame->child) + ", parent=" + FrameName(parent) + ", next=" + FrameName(frame->next) + " ]";
}

