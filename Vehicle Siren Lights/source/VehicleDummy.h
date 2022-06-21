#pragma once

#include "pch.h"

class VehicleDummy {
private:
	static void FindDummies(CVehicle* vehicle, RwFrame* frame);
public:
	static std::vector<RwFrame*> m_Frames;

	static std::vector<RwFrame*> GetFramesOnVehicle(CVehicle* vehicle);
	static std::vector<RwFrame*> GetFrameHierarchy(RwFrame* frame, RwFrame* root);

	static CVector GetTransformedPosition(CVehicle* vehicle, CVector position);
	static CVector GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset);
	static CVector FindTransformedDummyPosition(CVehicle* vehicle, std::string dummyName, CVector offset);
};