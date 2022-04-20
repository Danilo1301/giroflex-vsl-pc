#pragma once

#include "pch.h"
#include "PatternLoop.h"
#include "LightGroup.h"
#include "Mod.h"


class VehiclePatternData {
public:
	PatternLoop* patternLoop;
	PatternLoop* stepLoop;
	bool enabled = true;

	VehiclePatternData() {
		patternLoop = new PatternLoop();
		stepLoop = new PatternLoop();
	}
};

class Vehicle {
public:
	CVehicle* m_Vehicle;

	std::map<LightGroup*, VehiclePatternData*> m_LightGroupData;

	Vehicle(CVehicle* veh);

	void Update();
	void Draw();
	void DrawDebug();
	void DrawFrames();
	void DrawPoints();
	void Destroy();

	void RenderBefore();
	void RenderAfter();

	void UpdateVehicleMaterial(RpMaterial* material, std::string frameName);

	void RegisterCorona(int lightid, CVector position, CRGBA color, float radius);

	void CheckForLightGroups();
	void UpdatePatternAndSteps();

	void RegisterCoronas();
};