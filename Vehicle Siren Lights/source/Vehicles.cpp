#include "Vehicles.h"
#include "LightGroups.h"

std::map<CVehicle*, Vehicle*> Vehicles::m_Vehicles;
bool Vehicles::m_DrawVehicleFrames = false;
bool Vehicles::m_DrawVehiclePoints = false;

bool Vehicles::HasVehicle(CVehicle* veh) {
	return m_Vehicles.find(veh) != m_Vehicles.end();
}

Vehicle* Vehicles::AddVehicle(CVehicle* veh) {
	Log::file << "[Vehicles] AddVehicle " << veh << " [" << std::to_string(veh->m_nModelIndex) << "] (" << std::to_string(m_Vehicles.size()+1) << " total)" << std::endl;

	Vehicle* vehicle = new Vehicle(veh);
	m_Vehicles.insert(std::pair<CVehicle*, Vehicle*>(veh, vehicle));

	return vehicle;
}

Vehicle* Vehicles::GetVehicle(CVehicle* veh)
{
	if (!HasVehicle(veh)) return NULL;
	return m_Vehicles[veh];
}

void Vehicles::TryAddAllVehicles() {
	//Log::file << "[Vehicles] TryAddAllVehicles" << std::endl;

	for (auto veh : CPools::ms_pVehiclePool)
	{
		if (LightGroups::HasLightGroups(veh->m_nModelIndex)) {
			if (!HasVehicle(veh)) AddVehicle(veh);
		}
		else {
			if (HasVehicle(veh)) RemoveVehicle(veh);
		}
	}
}

void Vehicles::RemoveVehicle(CVehicle* veh) {
	Log::file << "[Vehicles] RemoveVehicle " << veh << " (" << std::to_string(m_Vehicles.size()-1) << " total)" << std::endl;

	Vehicle* vehicle = m_Vehicles[veh];
	m_Vehicles.erase(veh);
	vehicle->Destroy();

	delete vehicle;

}

void Vehicles::RemoveAllVehicles() {
	Log::file << "[Vehicles] RemoveAllVehicles" << std::endl;

	std::vector<CVehicle*> toremove;
	for (auto pair : m_Vehicles) toremove.push_back(pair.first);

	for (auto veh : toremove) RemoveVehicle(veh);
}