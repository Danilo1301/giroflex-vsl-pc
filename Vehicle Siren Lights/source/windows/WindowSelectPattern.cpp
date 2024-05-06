#include "WindowSelectPattern.h"

#include "../Vehicles.h"
#include "../menu/TextEditor.h"
#include "../localization/Localization.h"

std::vector<PatternCycleStep*>* WindowSelectPattern::m_PatternCycleSteps = nullptr;
PatternCycleStep* WindowSelectPattern::m_PatternCycleStep = nullptr;
LightGroup* WindowSelectPattern::m_LightGroup = nullptr;

Window* WindowSelectPattern::m_MainWindow = nullptr;

std::function<void()> WindowSelectPattern::m_OnBack;
std::function<void(Pattern*, int)> WindowSelectPattern::m_OnAddPatternCycleStep;
std::function<void(PatternCycleStep*)> WindowSelectPattern::m_OnDeletePatternCycleStep;

void WindowSelectPattern::CreatePatterns() {
	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "Patterns");

	if (m_MainWindow) {
		Menu::RemoveWindow(m_MainWindow);
	}

	m_MainWindow = window;

	auto patternCycleSteps = *m_PatternCycleSteps;

	if (patternCycleSteps.size() > 0) {
		for (auto patternCycleStep : patternCycleSteps)
		{
			auto button = window->AddButton(Localization::GetLineFormatted("select_edit_pattern", patternCycleStep->pattern->name, patternCycleStep->duration));
			button->m_OnClick = [window, patternCycleStep]() mutable {

				WindowSelectPattern::m_PatternCycleStep = patternCycleStep;
				WindowSelectPattern::RemoveMainWindow();
				WindowSelectPattern::CreateEditPattern();
			};
		}
	}

	auto addPattern = window->AddButton(Localization::GetLine("add_pattern"));
	addPattern->m_OnClick = [window]() {
		WindowSelectPattern::RemoveMainWindow();
		WindowSelectPattern::CreateAddPattern();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);

		if (m_OnBack) m_OnBack();
	};
	window->SetBackButton(buttonBack);
}

void WindowSelectPattern::CreateAddPattern() {
	auto window = Menu::AddWindow(Localization::GetLine("add_pattern"));

	//

	static Pattern* pattern;
	pattern = Patterns::m_Patterns[0];

	static std::string patternName;
	patternName = pattern->name;

	auto buttonObjName = window->AddButton(Localization::GetLine("select_pattern"));
	buttonObjName->AddTextStr(&patternName, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window]() {

		auto window2 = Menu::AddWindow("", Localization::GetLine("select_pattern"));
		window2->m_Position.x += window2->m_Size.x + 5.0f;

		int i = 0;
		for (auto selectPattern : Patterns::m_Patterns) {

			auto button = window2->AddButton(selectPattern->name);
			button->m_OnClick = [window, window2, selectPattern]() {
				pattern = selectPattern;
				patternName = selectPattern->name;

				Menu::RemoveWindow(window2);
				Menu::m_ActiveWindow = window;
			};

			i++;
		}
	};

	//

	static int timeVal = 0;
	timeVal = 5000;

	auto time = window->AddNumberRange(Localization::GetLine("time"), &timeVal, 1, 999999);
	time->m_OnClick = []() mutable {
		TextEditor::Open(Localization::GetLine("time"), true, &timeVal);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	//

	auto add = window->AddButton(Localization::GetLine("add"));
	add->m_OnClick = [window]() {
		if (m_OnAddPatternCycleStep) m_OnAddPatternCycleStep(pattern, timeVal);

		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto cancel = window->AddButton(Localization::GetLine("cancel"));
	cancel->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreatePatterns();
	};
}


void WindowSelectPattern::CreateEditPattern() {
	auto window = Menu::AddWindow("Edit pattern");

	auto patternCycleStep = m_PatternCycleStep;

	static std::string patternName;
	patternName = patternCycleStep->pattern->name;

	auto buttonObjName = window->AddButton(Localization::GetLine("select_pattern"));
	buttonObjName->AddTextStr(&patternName, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window, patternCycleStep]() {

		auto window2 = Menu::AddWindow("", Localization::GetLine("select_pattern"));
		window2->m_Position.x += window2->m_Size.x + 5.0f;

		int i = 0;
		for (auto selectPattern : Patterns::m_Patterns) {

			auto button = window2->AddButton(selectPattern->name);
			button->m_OnClick = [patternCycleStep, window, window2, selectPattern]() {
				Vehicles::RemoveAllVehicles();

				patternCycleStep->pattern = selectPattern;
				patternName = selectPattern->name;

				Menu::RemoveWindow(window2);
				Menu::m_ActiveWindow = window;
			};

			i++;
		}
	};

	auto time = window->AddNumberRange(Localization::GetLine("time"), &patternCycleStep->duration, 1, 999999);
	time->m_OnClick = [patternCycleStep]() mutable {
		TextEditor::Open(Localization::GetLine("time"), false, &patternCycleStep->duration);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_OnClick = [window, patternCycleStep]() {
		if (m_OnDeletePatternCycleStep) m_OnDeletePatternCycleStep(patternCycleStep);
		
		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreatePatterns();
	};
	window->SetBackButton(buttonBack);
}

void WindowSelectPattern::Close() {
	m_OnAddPatternCycleStep = NULL;
	m_OnBack = NULL;

	RemoveMainWindow();
}

void WindowSelectPattern::RemoveMainWindow() {
	Menu::RemoveWindow(m_MainWindow);
	m_MainWindow = nullptr;
}