#pragma once

#include "Pattern.h"

struct PatternCycleStep {
	Pattern* pattern;
	int duration;
	bool lerpColor;
};