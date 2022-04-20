#pragma once

#include "pch.h"

class VehicleDummy {
private:
	static void FindDummies(CVehicle* vehicle, RwFrame* frame, bool parent = false);
public:
	static std::vector<RwFrame*> m_Frames;

	static std::vector<RwFrame*> GetFramesOnVehicle(CVehicle* vehicle);
};