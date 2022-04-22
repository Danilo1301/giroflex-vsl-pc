#pragma once

#include "../LightGroups.h"
#include "../Patterns.h"
#include "../menu/Menu.h"

class WindowPattern {
public:
	static Pattern* m_Pattern;
	static PatternStep* m_PatternStep;

	static void CreatePatterns();
	static void CreateEditPattern();
	static void CreateEditStep();
};
