#pragma once

#include "pch.h"

struct PatternStep {
	std::vector<int> values;
	int duration;

	//bool leftState;
	//CRGBA leftColor;

	//bool middleState;
	//CRGBA middleColor;

	//bool rightState;
	//CRGBA rightColor;

	
};

class Pattern {
public:
	std::vector<PatternStep*> steps;
	std::string name;

	Pattern(std::string name)
	{
		this->name = name;
	}
	
	PatternStep* AddStep(std::vector<int> values, int duration)
	{
		PatternStep* step = new PatternStep();
		for (size_t i = 0; i < values.size(); i++)
		{
			step->values.push_back(values[i]);
		}
		step->duration = duration;
		steps.push_back(step);
		return step;
	}

	PatternStep* AddStep(bool left, bool middle, bool right, int duration)
	{
		PatternStep* step = new PatternStep();
		step->values.push_back(left ? 1 : 0);
		step->values.push_back(middle ? 1 : 0);
		step->values.push_back(right ? 1 : 0);
		step->duration = duration;
		steps.push_back(step);
		return step;
	}

	void RemoveStep(PatternStep* step)
	{
		steps.erase(std::find(steps.begin(), steps.end(), step));
		delete step;
	}

	int GetMaxTime()
	{
		int time = 0;
		for (PatternStep* step : steps) time += step->duration;
		return time;
	}

	/*
	Json::Value ToJSON(){
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
	*/
};