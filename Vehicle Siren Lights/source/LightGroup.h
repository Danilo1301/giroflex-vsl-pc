#pragma once

#include "pch.h"

#include "Patterns.h"
#include "Point.h"
#include "Keybinds.h"

#include <filesystem>

struct LightbarSettings {
	bool useLightbarLeds = false;
	std::string object_prefix = "lightbar-led-";
	CRGBA ledOnColor = CRGBA(255, 255, 255);
	CRGBA ledOffColor = CRGBA(30, 0, 0);
};

static double arch_fn_parabola(float x, float curve, float len)
{
	return -(pow(x, 2) * (double)curve) + (((double)len * (double)curve) * (double)x);
}

class LightGroup {
public:
	std::string fileName = "";
	int modelId;
	std::filesystem::directory_entry modelDirectoryEntry;

	LightbarSettings lightbarSettings;

	Keybind keybindMenu = Keybind("Z", KEYBIND_FLAGS::CTRL);

	std::string name = "Light group";

	bool isPositionLightGroup = false;

	CRGBA color1 = CRGBA(255, 0, 0);
	CRGBA color2 = CRGBA(0, 0, 255);
	CRGBA color3 = CRGBA(255, 255, 255);

	bool reflect = false;
	float reflectionIntensity = 0.5f;
	float reflectionDistance = 50.0f;
	bool turnOnSiren = true;
	float size = 0.2f;
	float distance = 0.3f;
	float curve = 0.0f;
	float nearClip = 0.3f;
	eCoronaFlareType flareType = eCoronaFlareType::FLARETYPE_NONE;
	float flareDistance = 150.0;
	float flareIntensity = 1.0f;
	eCoronaType type = eCoronaType::CORONATYPE_SHINYSTAR;
	eSirenDirection direction = eSirenDirection::BOTH;
	bool usePatternColors = false;
	CVector position = CVector(0, 0, 0);
	std::vector<Point*> points;
	bool pointsPositionFixed = false;
	std::vector<PatternCycleStep*> patternCycleSteps;
	int patternOffset = 0;
	bool useSmallWhiteCorona = true;
	float smallWhiteCoronaScale = 0.4f;
	eCoronaType smallWhiteCoronaType = eCoronaType::CORONATYPE_SHINYSTAR;
	bool freezeLights = false;
	bool alwaysEnabled = false;

	LightGroup(int modelId);

	Point* AddPoint();
	Point* AddPoint(eSirenPosition sirenPosition);
	void RemovePoint(Point* point);
	void SetNumberOfPoints(int amount);

	void AddPatternCycleStep(Pattern* pattern, int duration);
	void RemovePatternCycleStep(PatternCycleStep* patternCycleStep);
	void RemoveAllPatternCycleSteps();

	void FindNewPatterns();

	void Destroy();

	void RemoveAllPoints();

	Json::Value ToJSON();
	void FromJSON(Json::Value value);
};