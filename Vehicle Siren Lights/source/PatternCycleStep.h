#pragma once

#include "Patterns.h"
#include "Pattern.h"

struct PatternCycleStep {
	Pattern* pattern;
	int duration;
	bool lerpColor;
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

	int index = value["pattern"].asInt();

	if (index <= (int)Patterns::m_Patterns.size() - 1) {
		patternCycleStep->pattern = Patterns::m_Patterns[value["pattern"].asInt()];
	}
	else {
		patternCycleStep->pattern = nullptr;
	}

	patternCycleStep->duration = value["duration"].asInt();
	patternCycleStep->lerpColor = value["lerpColor"].asBool();

	return patternCycleStep;
}