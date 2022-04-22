#pragma once

#include "pch.h"

struct PatternStep {
	bool leftState;
	CRGBA leftColor;

	bool middleState;
	CRGBA middleColor;

	bool rightState;
	CRGBA rightColor;

	int duration;
};

class Pattern {
public:
	std::vector<PatternStep*> steps;
	std::string name;

	Pattern(std::string name) {
		this->name = name;
	}

	PatternStep* AddStep(bool left, bool middle, bool right, CRGBA color, int duration) {
		return AddStep(left, middle, right, color, color, color, duration);
	}

	PatternStep* AddStep(bool left, bool middle, bool right, CRGBA leftColor, CRGBA middleColor, CRGBA rightColor, int duration) {
		PatternStep* step = new PatternStep();
		step->leftState = left;
		step->middleState = middle;
		step->rightState = right;
		step->leftColor = leftColor;
		step->middleColor = middleColor;
		step->rightColor = rightColor;
		step->duration = duration;
		steps.push_back(step);
		return step;
	}

	void RemoveStep(PatternStep* step) {
		steps.erase(std::find(steps.begin(), steps.end(), step));
		delete step;
	}

	int GetMaxTime() {
		int time = 0;
		for (PatternStep* step : steps) time += step->duration;
		return time;
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;
		value["name"] = name;

		value["steps"] = Json::arrayValue;
		for (auto step : steps) {
			Json::Value stepValue;
			stepValue["duration"] = step->duration;

			stepValue["leftColor"] = ColorToJSON(step->leftColor);
			stepValue["middleColor"] = ColorToJSON(step->middleColor);
			stepValue["rightColor"] = ColorToJSON(step->rightColor);

			stepValue["leftState"] = step->leftState;
			stepValue["middleState"] = step->middleState;
			stepValue["rightState"] = step->rightState;

			value["steps"].append(stepValue);
		}

		return value;
	}

	void FromJSON(Json::Value value) {
		name = value["name"].asString();

		for (size_t i = 0; i < value["steps"].size(); i++)
		{
			Json::Value stepValue = value["steps"][i];

			AddStep(
				stepValue["leftState"].asBool(),
				stepValue["middleState"].asBool(),
				stepValue["rightState"].asBool(),
				ColorFromJSON(stepValue["leftColor"]),
				ColorFromJSON(stepValue["middleColor"]),
				ColorFromJSON(stepValue["rightColor"]),
				stepValue["duration"].asInt()
			);
		}
	}
};

struct PatternCycleStep {
	Pattern* pattern;
	int duration;
	bool lerpColor;
};