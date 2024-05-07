#pragma once

#include "../LightGroups.h"
#include "../Patterns.h"
#include "../PatternCycleStep.h"
#include "../menu/Menu.h"

class WindowSelectPattern {
public:
	static std::vector<PatternCycleStep*>* m_PatternCycleSteps;
	static PatternCycleStep* m_PatternCycleStep;
	static LightGroup* m_LightGroup;

	static Window* m_MainWindow;

	//static std::function<void()> m_OnAdd;
	static std::function<void()> m_OnBack;
	static std::function<void(Pattern*, int)> m_OnAddPatternCycleStep;
	static std::function<void(PatternCycleStep*)> m_OnDeletePatternCycleStep;

	static void CreatePatterns();
	static void CreateAddPattern();
	static void CreateEditPattern();
	
	static void Close();
	static void RemoveMainWindow();
};
