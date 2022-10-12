#pragma once

#include "pch.h"
#include "PatternLoop.h"
#include "LightGroup.h"
#include "VehiclePatternData.h"

class Vehicle {
public:
	static float m_MatAmbient;
	static bool m_FreezeLights;
	CVehicle* m_Vehicle;

	std::map<LightGroup*, VehiclePatternData*> m_LightGroupData;

	Vehicle(CVehicle* veh);

	bool m_PrevLightState = false;
	bool m_PrevSirenState = false;
	float m_RotateShadowAngle = 0;

	void Update();
	void UpdateSirenState();
	void Draw();
	void DrawDebug();
	void DrawFrames();
	void DrawPoints();
	void Destroy();

	void ResetObjectRotation(std::string object);
	void GoToNextLightGroupPattern(LightGroup* lightGroup);
	bool GetSirenState();

	void RenderBefore();
	void RenderAfter();

	void RenderShadows();

	void UpdateVehicleMaterial(RpMaterial* material, std::string frameName);
	void SetAllLightGroupState(bool enabled);

	void CheckForLightGroups();
	void UpdatePatternAndSteps();

	void RegisterCoronas();
};