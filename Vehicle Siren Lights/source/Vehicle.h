#pragma once

#include "pch.h"
#include "LightGroup.h"
#include "VehiclePatternData.h"

class Vehicle {
public:
	CVehicle* m_Vehicle;
	std::map<LightGroup*, VehiclePatternData*> m_LightGroupData;

	Vehicle(CVehicle* veh);

	void Update();
	void Draw();
	void Destroy();

	void CheckForLightGroups();
	void UpdatePatternAndSteps();
	void RegisterCoronas();

	void GoToNextLightGroupPattern(LightGroup* lightGroup);

	void RenderBefore();
	void RenderAfter();

};