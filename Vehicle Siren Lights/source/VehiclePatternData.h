#pragma once

#include "pch.h"
#include "PatternLoop.h"

class VehiclePatternData {
public:
	PatternLoop* patternLoop;
	PatternLoop* stepLoop;
	bool lightsOn = false;
	bool paused = false;
	bool autoChangePattern = true;

	VehiclePatternData() {
		patternLoop = new PatternLoop();
		patternLoop->Name = "Pattern";

		stepLoop = new PatternLoop();
		stepLoop->Name = "Step";
	}

	~VehiclePatternData()
	{
		delete patternLoop;
		delete stepLoop;
	}
};