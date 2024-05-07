#pragma once

#include "pch.h"

#include <filesystem>

struct PatternStep {
	std::vector<int> values;
	int duration;
};

class Pattern {
public:
	std::vector<PatternStep*> steps;
	std::string name;
	std::string fileName;
	std::filesystem::directory_entry directoryEntry;

	Pattern(std::string name);

	PatternStep* AddStep(std::vector<int> values, int duration);
	//void RemoveStep(PatternStep* step);

	int GetMaxTime();

	Json::Value ToJSON();
	void FromJSON(Json::Value value);
};