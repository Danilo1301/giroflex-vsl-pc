#pragma once

#include "pch.h"
#include "Pattern.h"

class Patterns {
public:
	static std::vector<Pattern*> m_Patterns;

	static Pattern* CreatePattern(std::string name);
	static void RemovePattern(Pattern* pattern);
	static void RemoveAllPatterns();
};

static Json::Value PatternCycleStepToJSON(PatternCycleStep* patternCycleStep) {
	Json::Value value = Json::objectValue;

	auto index = (std::find(Patterns::m_Patterns.begin(), Patterns::m_Patterns.end(), patternCycleStep->pattern) - Patterns::m_Patterns.begin());

	value["pattern"] = index;
	value["duration"] = patternCycleStep->duration;
	value["lerpColor"] = patternCycleStep->lerpColor;

	return value;
}

static PatternCycleStep* PatternCycleStepFromJSON(Json::Value value) {
	PatternCycleStep* patternCycleStep = new PatternCycleStep();
	patternCycleStep->pattern = Patterns::m_Patterns[value["pattern"].asInt()];
	patternCycleStep->duration = value["duration"].asInt();
	patternCycleStep->lerpColor = value["lerpColor"].asBool();

	return patternCycleStep;
}