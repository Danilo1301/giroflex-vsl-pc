#pragma once

#include "pch.h"
#include "Pattern.h"

class Patterns {
public:
	static std::vector<Pattern*> m_Patterns;

	static Pattern* CreatePattern(std::string name);
	static Pattern* GetPatternByFileName(std::string fileName);
	static void RemovePattern(Pattern* pattern);
	static void RemoveAllPatterns();
};

static Json::Value PatternCycleStepToJSON(PatternCycleStep* patternCycleStep) {
	Json::Value value = Json::objectValue;

	//auto index = (std::find(Patterns::m_Patterns.begin(), Patterns::m_Patterns.end(), patternCycleStep->pattern) - Patterns::m_Patterns.begin());

	value["pattern"] = patternCycleStep->pattern->fileName;
	value["duration"] = patternCycleStep->duration;
	value["lerpColor"] = patternCycleStep->lerpColor;

	return value;
}

static PatternCycleStep* PatternCycleStepFromJSON(Json::Value value) {
	PatternCycleStep* patternCycleStep = new PatternCycleStep();

	Pattern* pattern = NULL;

	if (value["pattern"].isNumeric())
	{
		int index = value["pattern"].asInt();
		if (index <= (int)Patterns::m_Patterns.size() - 1) {
			pattern = Patterns::m_Patterns[value["pattern"].asInt()];
		}
	}
	else {
		pattern = Patterns::GetPatternByFileName(value["pattern"].asString());
	}

	patternCycleStep->pattern = pattern;
	patternCycleStep->duration = value["duration"].asInt();
	patternCycleStep->lerpColor = value["lerpColor"].asBool();

	return patternCycleStep;
}