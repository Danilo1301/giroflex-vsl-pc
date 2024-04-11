#pragma once

#include "pch.h"

#include "Patterns.h"
#include "Point.h"
#include "LightGroupShadow.h"
#include "Keybinds.h"
#include "TestHelper.h"
#include "Mod.h"

#include "log/Log.h"

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
	LightbarSettings lightbarSettings;

	//Keybind keybindChange = Keybind("", 0);
	Keybind keybindMenu = Keybind("Z", KEYBIND_FLAGS::CTRL);

	std::string name = "Light group";
	std::string fileName = "";
	int modelId;

	CRGBA color1 = CRGBA(255, 0, 0);
	CRGBA color2 = CRGBA(0, 0, 255);
	CRGBA color3 = CRGBA(255, 255, 255);

	bool reflect = false;
	bool turnOnSiren = true;
	float reflectionDistance = 20.0f;
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
	std::vector<PatternCycleStep*> patternCycleSteps;
	int patternOffset = 0;
	int offsetId = 0;
	bool useSmallWhiteCorona = true;
	float smallWhiteCoronaScale = 0.4f;
	eCoronaType smallWhiteCoronaType = eCoronaType::CORONATYPE_SHINYSTAR;

	LightGroup(int modelId) {
		this->modelId = modelId;
		this->keybindMenu.SaveAsNumber = true;
	}

	Point* AddPoint() {
		Point* point = new Point();
		points.push_back(point);
		return point;
	}

	void RemovePoint(Point* point) {
		auto it = std::find(points.begin(), points.end(), point);
		if (it == points.end()) return;
		points.erase(it);
		delete point;
	}

	void SetNumberOfPoints(int amount)
	{
		while ((int)points.size() < amount)
		{
			AddPoint();
		}
		while ((int)points.size() > amount)
		{
			RemovePoint(points[points.size() - 1]);
		}
	}

	void AddPatternCycleStep(Pattern* pattern, int duration) {
		PatternCycleStep* patternCycleStep = new PatternCycleStep();
		patternCycleStep->duration = duration;
		patternCycleStep->lerpColor = false;
		patternCycleStep->pattern = pattern;
		patternCycleSteps.push_back(patternCycleStep);
	}

	void RemovePatternCycleStep(PatternCycleStep* patternCycleStep) {
		Log::file << "[LightGroup : " << name << "] Pattern '" << patternCycleStep->pattern->name << "' removed" << std::endl;

		auto it = std::find(patternCycleSteps.begin(), patternCycleSteps.end(), patternCycleStep);
		if (it == patternCycleSteps.end()) return;
		patternCycleSteps.erase(it);
		delete patternCycleStep;
	}

	void RemoveAllPatternCycleSteps()
	{
		while (patternCycleSteps.size() > 0)
			RemovePatternCycleStep(patternCycleSteps[0]);
	}

	void FindNewPatterns()
	{
		RemoveAllPatternCycleSteps();

		for (auto pattern : Patterns::m_Patterns)
		{
			if (pattern->steps[0]->values.size() == points.size())
			{
				AddPatternCycleStep(pattern, 7000);
			}
		}
	}

	void Destroy() {
		RemoveAllPatternCycleSteps();
		
		RemoveAllPoints();
	}

	void RemoveAllPoints()
	{
		while (points.size() > 0)
			RemovePoint(points[0]);
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;

		value["lightbarSettings"] = Json::objectValue;
		value["lightbarSettings"]["useLightbarLeds"] = lightbarSettings.useLightbarLeds;
		value["lightbarSettings"]["object_prefix"] = lightbarSettings.object_prefix;
		value["lightbarSettings"]["ledOnColor"] = ColorToJSON(lightbarSettings.ledOnColor);
		value["lightbarSettings"]["ledOffColor"] = ColorToJSON(lightbarSettings.ledOffColor);

		value["keybindMenu"] = keybindMenu.ToJSON();

		value["name"] = name;

		value["color1"] = ColorToJSON(color1);
		value["color2"] = ColorToJSON(color2);
		value["color3"] = ColorToJSON(color3);

		value["reflect"] = reflect;
		value["turnOnSiren"] = turnOnSiren;
		value["reflectionDistance"] = reflectionDistance;
		value["size"] = size;
		value["distance"] = distance;
		value["curve"] = curve;
		value["nearClip"] = nearClip;
		value["flareType"] = (int)flareType;
		value["flareDistance"] = flareDistance;
		value["flareIntensity"] = flareIntensity;
		value["type"] = (int)type;
		value["direction"] = (int)direction;
		value["usePatternColors"] = usePatternColors;
		value["position"] = CVectorToJSON(position);
		value["patternOffset"] = patternOffset;
		value["offsetId"] = offsetId;
		value["useSmallWhiteCorona"] = useSmallWhiteCorona;
		value["smallWhiteCoronaScale"] = smallWhiteCoronaScale;
		value["smallWhiteCoronaType"] = (int)smallWhiteCoronaType;

		value["points"] = Json::arrayValue;
		for (auto point : points) {
			value["points"].append( point->ToJSON() );
		}

		value["patternCycleSteps"] = Json::arrayValue;
		for (auto patternCycleStep : patternCycleSteps) {
			value["patternCycleSteps"].append( PatternCycleStepToJSON(patternCycleStep) );
		}

		value["version"] = Mod::m_Version;

		return value;
	}

	void FromJSON(Json::Value value) {
		name = value["name"].asString();

		if (!value["lightbarSettings"].isNull())
		{
			lightbarSettings.useLightbarLeds = ValidateValue(value["lightbarSettings"]["useLightbarLeds"], lightbarSettings.useLightbarLeds).asBool();
			lightbarSettings.object_prefix = value["lightbarSettings"]["object_prefix"].asString();
			lightbarSettings.ledOnColor = ColorFromJSON(value["lightbarSettings"]["ledOnColor"]);
			lightbarSettings.ledOffColor = ColorFromJSON(value["lightbarSettings"]["ledOffColor"]);
		}

		if (!value["keybindMenu"].isNull())
		{
			keybindMenu.FromJSON(value["keybindMenu"]);
		}

		color1 = ValidateColor(value["color1"], color1);
		color2 = ValidateColor(value["color2"], color2);
		color3 = ValidateColor(value["color3"], color3);

		reflect = value["reflect"].asBool();
		turnOnSiren = value["turnOnSiren"].asBool();
		reflectionDistance = value["reflectionDistance"].asFloat();
		size = value["size"].asFloat();
		distance = value["distance"].asFloat();
		curve = value["curve"].asFloat();
		nearClip = value["nearClip"].asFloat();
		flareType = (eCoronaFlareType)value["flareType"].asInt();
		flareDistance = value["flareDistance"].asFloat();
		flareIntensity = value["flareIntensity"].asFloat();
		type = (eCoronaType)value["type"].asInt();
		direction = (eSirenDirection)value["direction"].asInt();
		usePatternColors = value["usePatternColors"].asBool();
		position = CVectorFromJSON(value["position"]);
		patternOffset = ValidateValue(value["patternOffset"], patternOffset).asInt();
		offsetId = ValidateValue(value["offsetId"], offsetId).asInt();
		useSmallWhiteCorona = ValidateValue(value["useSmallWhiteCorona"], useSmallWhiteCorona).asBool();
		smallWhiteCoronaScale = ValidateValue(value["smallWhiteCoronaScale"], smallWhiteCoronaScale).asFloat();
		smallWhiteCoronaType = (eCoronaType)ValidateValue(value["smallWhiteCoronaType"], (int)smallWhiteCoronaType).asInt();

		for (size_t i = 0; i < value["points"].size(); i++)
		{
			Json::Value pointValue = value["points"][i];

			Point* point = new Point();
			point->FromJSON(pointValue);
			points.push_back(point);
		}

		for (size_t i = 0; i < value["patternCycleSteps"].size(); i++)
		{
			Json::Value patternCycleStepValue = value["patternCycleSteps"][i];

			PatternCycleStep* patternCycleStep = PatternCycleStepFromJSON(patternCycleStepValue);

			if (patternCycleStep->pattern == nullptr) {
				Log::file << "[LightGroup] Pattern not found, patternCycleStep not added" << std::endl;

				delete patternCycleStep;
			}
			else {
				patternCycleSteps.push_back(patternCycleStep);
			}

			
		}
	}
};