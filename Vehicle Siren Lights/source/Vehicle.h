#pragma once

#include "pch.h"
#include "LightGroup.h"
#include "ColorTexture.h"

struct LightGroupData {
	LightGroup* lightGroup;
	int patternTime = 0;
	int patternProgress = 0;
	int patternIndex = 0;
	bool enabled = true;
};


struct ColorTextureData {
	ColorTexture* colorTexture;
	int patternTime = 0;
	int patternProgress = 0;
	int patternIndex = 0;
	bool enabled = true;
};


static PatternData* GetLightGroupPatternData(LightGroupData* lightGroupData) {
	return lightGroupData->lightGroup->patternsData[lightGroupData->patternIndex];
}

static PatternData* GetColorTexturePatternData(ColorTextureData* colorTextureData) {
	return colorTextureData->colorTexture->patternsData[colorTextureData->patternIndex];
}

class Vehicle {
public:
	CVehicle* m_Vehicle;

	std::map<LightGroup*, LightGroupData*> m_LightGroupData;
	std::map<ColorTexture*, ColorTextureData*> m_ColorTextureData;

	CObject* m_TestObject = nullptr; //delete later

	bool m_PrevSirenState = false;

	Vehicle(CVehicle* veh);

	void Update();
	void Draw();
	void DrawDebug();
	void Destroy();
	void RenderBefore();
	void RenderAfter();

	void RemoveLightGroupData();
	void RemoveColorTextureData();

	void RegisterCorona(int lightid, CVector position, CRGBA color, float radius);
};

class Vehicles {
public:
	static std::map<CVehicle*, Vehicle*> m_Vehicles;

	

	/*
	static void RemoveAllPatternsFromVehicle(CVehicle* veh) {
		if (!HasVehicle(veh)) return;

		auto vehicle = m_Vehicles[veh];
		vehicle->RemoveColorTextureData();
		vehicle->RemoveLightGroupData();
	}
	*/

	static bool HasVehicle(CVehicle* veh) {
		return m_Vehicles.find(veh) != m_Vehicles.end();
	}

	static Vehicle* AddVehicle(CVehicle* veh) {
		Log::file << "[Vehicles] Add vehicle " << veh << "[" << std::to_string(veh->m_nModelIndex) << "] ( " << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;
		
		Vehicle* vehicle = new Vehicle(veh);
		m_Vehicles.insert(std::pair<CVehicle*, Vehicle*>(veh, vehicle));


		return vehicle;
	}

	static void TryAddAllVehicles() {
		Log::file << "[Vehicles] TryAddAllVehicles" << std::endl;

		for (auto veh : CPools::ms_pVehiclePool)
		{
			if (!HasVehicle(veh)) AddVehicle(veh);
		}
	}

	static void RemoveVehicle(CVehicle* veh) {
		Log::file << "[Vehicles] Remove vehicle " << veh << "(" << std::to_string(m_Vehicles.size() - 1) << " total)" << std::endl;

		Vehicle* vehicle = m_Vehicles[veh];
		m_Vehicles.erase(veh);
		vehicle->Destroy();

		delete vehicle;
	}

	static void RemoveAllVehicles() {
		Log::file << "[Vehicles] RemoveAllVehicles" << std::endl;

		std::vector<CVehicle*> toremove;

		for (auto pair : m_Vehicles) {
			toremove.push_back(pair.first);
		}

		for (auto veh : toremove) {
			RemoveVehicle(veh);
		}
	}
};