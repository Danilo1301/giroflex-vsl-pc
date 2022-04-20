#pragma once

#include "pch.h"
#include "Pattern.h"
#include "Pattern.h"

enum class eSirenPosition {
	LEFT,
	MIDDLE,
	RIGHT
};

class Point {
public:
	CVector position = CVector(0, 0, 0);
	std::string object = "objectName";
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
};

class LightGroup {
public:
	int modelId;
	bool reflect = true;
	float reflectionDistance = 10.0f;
	float size = 0.2f;
	std::string name = "Light group";
	CVector position = CVector(0, 0, 0);
	std::vector<Point*> points;
	std::vector<PatternCycleStep*> patternCycleSteps;

	LightGroup(int modelId) {
		this->modelId = modelId;
	}

	Point* AddPoint(CVector position, eSirenPosition sirenPosition) {
		Point* point = new Point();
		point->position = position;
		point->sirenPosition = sirenPosition;
		points.push_back(point);
		return point;
	}

	Point* AddPoint(CVector position) {
		return AddPoint(position, eSirenPosition::LEFT);
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

		Log::file << "Pattern removed to lightgroup" << std::endl;
	}

	void Destroy() {
		while (patternCycleSteps.size() > 0) 
			RemovePatternCycleStep(patternCycleSteps[0]);
		
		while (points.size() > 0) 
			RemovePoint(points[0]);
	}
};