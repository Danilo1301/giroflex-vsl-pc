#include "Pattern.h"

Pattern::Pattern(std::string name)
{
	this->name = name;
}

PatternStep* Pattern::AddStep(std::vector<int> values, int duration)
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

int Pattern::GetMaxTime()
{
	int time = 0;
	for (PatternStep* step : steps) time += step->duration;
	return time;
}

Json::Value Pattern::ToJSON()
{
	Json::Value value = Json::objectValue;
	//value["name"] = name;

	value["steps"] = Json::arrayValue;
	for (auto step : steps) {
		Json::Value stepValue;
		stepValue["duration"] = step->duration;

		stepValue["data"] = Json::arrayValue;
		for (auto val : step->values)
		{
			stepValue["data"].append(val);
		}

		value["steps"].append(stepValue);
	}

	return value;
}

void Pattern::FromJSON(Json::Value value)
{
	name = fileName;

	for (int step_i = 0; step_i < (int)value["steps"].size(); step_i++)
	{
		Json::Value step = value["steps"][step_i];

		int duration = step["duration"].asInt();

		if (duration <= 0) continue;

		std::vector<int> values;
		for (int val_i = 0; val_i < (int)step["data"].size(); val_i++)
		{
			values.push_back(step["data"][val_i].asInt());
		}

		AddStep(values, duration);
	}
}