#pragma once

#include "pch.h"

#include "Pattern.h"
#include "PatternCycleStep.h"
#include "Point.h"
#include "Keybinds.h"

struct LightbarSettings {
	int amountOfLights = 11;
	float lightDistance = 0.1f;
	std::string object_prefix = "giroflex_led_";
};

class LightGroup {
public:
	bool isLightbar = false;
	LightbarSettings lightbarSettings;

	std::string name = "Light group";
	int modelId;
	bool reflect = false;
	bool turnOnSiren = true;
	float reflectionDistance = 20.0f;
	float size = 0.2f;
	float nearClip = 0.8f;
	eCoronaFlareType flareType = eCoronaFlareType::FLARETYPE_NONE;
	eCoronaType type = eCoronaType::CORONATYPE_SHINYSTAR;
	eSirenDirection direction = eSirenDirection::BOTH;
	//bool usePatternColors = false;
	CVector position = CVector(0, 0, 0);
	std::vector<Point*> points;
	std::vector<PatternCycleStep*> patternCycleSteps;
	int offsetId = 0;

	Keybind keybindChange = Keybind("", 0);
	Keybind keybindPause = Keybind("", 0);

	LightGroup(int modelId, bool isLightbar) {
		this->modelId = modelId;
		this->isLightbar = isLightbar;

		UpdateLightbarPoints();
	}

	void UpdateLightbarPoints()
	{
		if (!isLightbar) return;

		if (points.size() == 0)
		{
			for (int i = 0; i < lightbarSettings.amountOfLights; i++)
			{
				CVector pos = CVector(i * lightbarSettings.lightDistance, 0, 0);

				AddPoint(pos, CRGBA(255, 0, 0));
			}
		}
	}

	Point* AddPoint(CVector position, CRGBA color, eSirenPosition sirenPosition)
	{
		Point* point = new Point();
		point->position = position;
		point->sirenPosition = sirenPosition;
		point->color = color;
		//point->shadow.enabled = true;
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
		auto it = std::find(patternCycleSteps.begin(), patternCycleSteps.end(), patternCycleStep);
		if (it == patternCycleSteps.end()) return;
		patternCycleSteps.erase(it);
		delete patternCycleStep;

		Log::file << "[LightGroup : " << name << "] Pattern '" << patternCycleStep->pattern->name << "' removed" << std::endl;
	}

	void Destroy() {
		while (patternCycleSteps.size() > 0)
			RemovePatternCycleStep(patternCycleSteps[0]);

		while (points.size() > 0)
			RemovePoint(points[0]);
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;

		value["name"] = name;
		value["reflect"] = reflect;
		value["turnOnSiren"] = turnOnSiren;
		value["reflectionDistance"] = reflectionDistance;
		value["size"] = size;
		value["nearClip"] = nearClip;
		value["flareType"] = (int)flareType;
		value["type"] = (int)type;
		value["direction"] = (int)direction;
		//value["usePatternColors"] = usePatternColors;
		value["position"] = CVectorToJSON(position);
		value["offsetId"] = offsetId;

		value["points"] = Json::arrayValue;
		for (auto point : points) {
			value["points"].append(point->ToJSON());
		}

		value["patternCycleSteps"] = Json::arrayValue;
		for (auto patternCycleStep : patternCycleSteps) {
			value["patternCycleSteps"].append(PatternCycleStepToJSON(patternCycleStep));
		}

		return value;
	}

	void FromJSON(Json::Value value) {
		name = value["name"].asString();
		reflect = value["reflect"].asBool();
		turnOnSiren = value["turnOnSiren"].asBool();
		reflectionDistance = value["reflectionDistance"].asFloat();
		size = value["size"].asFloat();
		nearClip = value["nearClip"].asFloat();
		flareType = (eCoronaFlareType)value["flareType"].asInt();
		type = (eCoronaType)value["type"].asInt();
		direction = (eSirenDirection)value["direction"].asInt();
		//usePatternColors = value["usePatternColors"].asBool();
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