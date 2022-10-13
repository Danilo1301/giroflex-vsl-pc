#pragma once

#include "pch.h"

#include "Patterns.h"
#include "Point.h"
#include "LightGroupShadow.h"
#include "Keybinds.h"

#include "log/Log.h"

struct LightbarSettings {
	//int amountOfLights = 11;
	//float lightDistance = 0.1f;

	bool isLightbar = false;
	std::string object_prefix = "lightbar-led-";
	CRGBA ledOnColor = CRGBA(255, 255, 0);
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
	bool reflect = false;
	bool turnOnSiren = true;
	float reflectionDistance = 20.0f;
	float size = 0.2f;
	float nearClip = 0.3f;
	eCoronaFlareType flareType = eCoronaFlareType::FLARETYPE_NONE;
	eCoronaType type = eCoronaType::CORONATYPE_SHINYSTAR;
	eSirenDirection direction = eSirenDirection::BOTH;
	bool usePatternColors = false;
	CVector position = CVector(0, 0, 0);
	std::vector<Point*> points;
	std::vector<PatternCycleStep*> patternCycleSteps;
	int offsetId = 0;

	LightGroup(int modelId) {
		this->modelId = modelId;
		this->keybindMenu.SaveAsNumber = true;
	}

	bool IsLightbar()
	{
		return lightbarSettings.isLightbar;
	}

	void UpdateLightbarPoints(int amountOfLights, float curve, float distance)
	{
		if (!IsLightbar()) return;

		CVector lastPos = CVector(0, 0, 0);
		CRGBA lastColor = CRGBA(255, 0, 0);

		int i = 0;
		while ((int)points.size() < amountOfLights)
		{
			Point* lastPoint = points.size() > 0 ? points[points.size() - 1] : NULL;

			if (lastPoint)
			{
				lastPos = lastPoint->position;
				lastColor = lastPoint->color;

				lastPos.x += distance;
			}

			lastPos.y = (float)arch_fn_parabola((float)i, curve, (float)(amountOfLights-1));

			AddPoint(lastPos, lastColor);

			i++;
		}

		while (amountOfLights < (int)points.size())
		{
			Point* lastPoint = points[points.size() - 1];
			RemovePoint(lastPoint);
		}
	}

	Point* AddPoint(CVector position, CRGBA color, eSirenPosition sirenPosition) {
		Point* point = new Point();
		point->position = position;
		point->sirenPosition = sirenPosition;
		point->color = color;
		//point->shadow.enabled = false;
		points.push_back(point);
		return point;
	}

	Point* AddPoint(CVector position, CRGBA color) {
		return AddPoint(position, color, eSirenPosition::LEFT);
	}

	void RemovePoint(Point* point) {
		auto it = std::find(points.begin(), points.end(), point);
		if (it == points.end()) return;
		points.erase(it);
		delete point;
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

	void Destroy() {
		while (patternCycleSteps.size() > 0) 
			RemovePatternCycleStep(patternCycleSteps[0]);
		
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
		value["lightbarSettings"] = Json::objectValue;
		value["lightbarSettings"]["isLightbar"] = lightbarSettings.isLightbar;
		value["lightbarSettings"]["object_prefix"] = lightbarSettings.object_prefix;
		value["lightbarSettings"]["ledOnColor"] = ColorToJSON(lightbarSettings.ledOnColor);
		value["lightbarSettings"]["ledOffColor"] = ColorToJSON(lightbarSettings.ledOffColor);

		value["keybindMenu"] = keybindMenu.ToJSON();

		value["name"] = name;
		value["reflect"] = reflect;
		value["turnOnSiren"] = turnOnSiren;
		value["reflectionDistance"] = reflectionDistance;
		value["size"] = size;
		value["nearClip"] = nearClip;
		value["flareType"] = (int)flareType;
		value["type"] = (int)type;
		value["direction"] = (int)direction;
		value["usePatternColors"] = usePatternColors;
		value["position"] = CVectorToJSON(position);
		value["offsetId"] = offsetId;

		value["points"] = Json::arrayValue;
		for (auto point : points) {
			value["points"].append( point->ToJSON() );
		}

		value["patternCycleSteps"] = Json::arrayValue;
		for (auto patternCycleStep : patternCycleSteps) {
			value["patternCycleSteps"].append( PatternCycleStepToJSON(patternCycleStep) );
		}

		return value;
	}

	void FromJSON(Json::Value value) {
		name = value["name"].asString();

		if (!value["lightbarSettings"].isNull())
		{
			lightbarSettings.isLightbar = value["lightbarSettings"]["isLightbar"].asBool();
			lightbarSettings.object_prefix = value["lightbarSettings"]["object_prefix"].asString();
			lightbarSettings.ledOnColor = ColorFromJSON(value["lightbarSettings"]["ledOnColor"]);
			lightbarSettings.ledOffColor = ColorFromJSON(value["lightbarSettings"]["ledOffColor"]);
		}

		if (!value["keybindMenu"].isNull())
		{
			keybindMenu.FromJSON(value["keybindMenu"]);
		}

		reflect = value["reflect"].asBool();
		turnOnSiren = value["turnOnSiren"].asBool();
		reflectionDistance = value["reflectionDistance"].asFloat();
		size = value["size"].asFloat();
		nearClip = value["nearClip"].asFloat();
		flareType = (eCoronaFlareType)value["flareType"].asInt();
		type = (eCoronaType)value["type"].asInt();
		direction = (eSirenDirection)value["direction"].asInt();
		usePatternColors = value["usePatternColors"].asBool();
		position = CVectorFromJSON(value["position"]);
		offsetId = ValidateValue(value["offsetId"], offsetId).asInt();

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