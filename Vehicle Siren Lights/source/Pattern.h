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

	int GetMaxTime() {
		int time = 0;
		for (PatternStep* step : steps) time += step->duration;
		return time;
	}
};

struct PatternCycleStep {
	Pattern* pattern;
	int duration;
	bool lerpColor;
};