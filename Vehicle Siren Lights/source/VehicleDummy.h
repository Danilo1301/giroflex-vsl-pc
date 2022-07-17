#pragma once

#include "pch.h"

class VehicleDummy {
private:
	static void FindDummies(CVehicle* vehicle, RwFrame* frame);
	static void CreateTempMatrix();
public:
	static std::vector<RwFrame*> m_Frames;

	static std::vector<RwFrame*> GetFramesOnVehicle(CVehicle* vehicle);
	static std::vector<RwFrame*> GetFrameHierarchy(RwFrame* frame, RwFrame* root);

	static RwFrame* FindDummy(CVehicle* vehicle, std::string dummyName);

	static CVector GetDummyOffset(CVehicle* vehicle, std::string dummyName);
	static CVector GetTransformedPosition(CVehicle* vehicle, CVector offset);
	static CVector GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset);
	static CVector GetTransformedDummyPositionByName(CVehicle* vehicle, std::string dummyName, CVector offset);
};