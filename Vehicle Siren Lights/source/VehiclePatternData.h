#pragma once

#include "pch.h"
#include "PatternLoop.h"

class VehiclePatternData {
public:
	PatternLoop* patternLoop;
	PatternLoop* stepLoop;
	bool lightsOn = true;
	bool paused = false;

	VehiclePatternData() {
		patternLoop = new PatternLoop();
		patternLoop->Name = "Pattern";

		stepLoop = new PatternLoop();
		stepLoop->Name = "Step";
	}
};