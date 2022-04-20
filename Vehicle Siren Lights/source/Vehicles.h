#pragma once

#include "pch.h"
#include "Vehicle.h"

class Vehicles {
public:
	static std::map<CVehicle*, Vehicle*> m_Vehicles;
	static bool m_DrawVehicleFrames;
	static bool m_DrawVehiclePoints;

	static bool HasVehicle(CVehicle* veh);
	static Vehicle* AddVehicle(CVehicle* veh);
	static void TryAddAllVehicles();
	static void RemoveVehicle(CVehicle* veh);
	static void RemoveAllVehicles();
};