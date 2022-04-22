#pragma once

#include "pch.h"
#include "Patterns.h"

enum class eSirenPosition {
	LEFT,
	MIDDLE,
	RIGHT
};

enum class eSirenDirection {
	FRONT,
	BOTH,
	BACK
};


class Point {
public:
	CVector position = CVector(0, 0, 0);
	std::string object = "chassis"; //CHANGE
	CRGBA color = CRGBA(255, 0, 0);
	CRGBA disabledColor = CRGBA(0, 0, 0);
	eSirenPosition sirenPosition = eSirenPosition::LEFT;

	CRGBA GetColor(PatternStep* step) {
		switch (sirenPosition)
		{
		case eSirenPosition::LEFT:
			return step->leftColor;
		case eSirenPosition::MIDDLE:
			return step->middleColor;
		case eSirenPosition::RIGHT:
			return step->rightColor;
		}
		return NULL;
	}

	bool GetIsEnabled(PatternStep* step) {
		switch (sirenPosition)
		{
		case eSirenPosition::LEFT:
			return step->leftState;
		case eSirenPosition::MIDDLE:
			return step->middleState;
		case eSirenPosition::RIGHT:
			return step->rightState;
		}

		return false;
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;

		value["position"] = CVectorToJSON(position);
		value["object"] = object;
		value["color"] = ColorToJSON(color);
		value["disabledColor"] = ColorToJSON(disabledColor);
		value["sirenPosition"] = (int)sirenPosition;

		return value;
	}

	void FromJSON(Json::Value value) {
		position = CVectorFromJSON(value["position"]);
		object = value["object"].asString();
		color = ColorFromJSON(value["color"]);
		disabledColor = ColorFromJSON(value["disabledColor"]);
		sirenPosition = (eSirenPosition)value["sirenPosition"].asInt();
	}
};

class LightGroup {
public:
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
	bool usePatternColors = false;
	CVector position = CVector(0, 0, 0);
	std::vector<Point*> points;
	std::vector<PatternCycleStep*> patternCycleSteps;

	LightGroup(int modelId) {
		this->modelId = modelId;
	}

	Point* AddPoint(CVector position, CRGBA color, eSirenPosition sirenPosition) {
		Point* point = new Point();
		point->position = position;
		point->sirenPosition = sirenPosition;
		point->color = color;
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

		Log::file << "[LightGroup] Pattern removed" << std::endl;
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
		value["usePatternColors"] = usePatternColors;
		value["position"] = CVectorToJSON(position);

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
			patternCycleSteps.push_back(patternCycleStep);
		}
	}
};