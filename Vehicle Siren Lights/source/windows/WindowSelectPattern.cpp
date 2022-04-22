#include "WindowSelectPattern.h"

#include "../menu/TextEditor.h"

#include "../Vehicles.h"

std::vector<PatternCycleStep*>* WindowSelectPattern::m_PatternCycleSteps = nullptr;
PatternCycleStep* WindowSelectPattern::m_PatternCycleStep = nullptr;
LightGroup* WindowSelectPattern::m_LightGroup = nullptr;

Window* WindowSelectPattern::m_MainWindow = nullptr;

//std::function<void()> WindowSelectPattern::m_OnAdd;
std::function<void()> WindowSelectPattern::m_OnBack;
std::function<void(Pattern*, int)> WindowSelectPattern::m_OnAddPatternCycleStep;
std::function<void(PatternCycleStep*)> WindowSelectPattern::m_OnDeletePatternCycleStep;

void WindowSelectPattern::CreatePatterns() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Patterns");

	if (m_MainWindow) {
		Menu::RemoveWindow(m_MainWindow);
	}

	m_MainWindow = window;

	auto patternCycleSteps = *m_PatternCycleSteps;

	if (patternCycleSteps.size() > 0) {
		for (auto patternCycleStep : patternCycleSteps)
		{
			auto button = window->AddButton("[ Time: " + std::to_string(patternCycleStep->duration) + ", Pattern: " + patternCycleStep->pattern->name + " ]");
			button->m_OnClick = [window, patternCycleStep]() mutable {

				WindowSelectPattern::m_PatternCycleStep = patternCycleStep;
				WindowSelectPattern::RemoveMainWindow();
				WindowSelectPattern::CreateEditPattern();


				/*
				CreateMenu_SelectPatterns_Edit(patternData, [patternData, lightGroup](bool remove) {

					

					if (remove) {
						Vehicles::RemoveAllVehicles();
						lightGroup->RemovePattern(patternData);
						Vehicles::TryAddAllVehicles();

						CMessages::AddMessageJumpQ("removed", 500, 0, false);
					}
					else {}

					CreateMenu_EditLightGroup_EditPatterns(lightGroup);

					});
				*/
			};
		}
	}

	auto addPattern = window->AddButton("Add pattern");
	addPattern->m_OnClick = [window]() {
		WindowSelectPattern::RemoveMainWindow();
		WindowSelectPattern::CreateAddPattern();

		//if (m_OnAdd) m_OnAdd();

		/*
		CreateMenu_SelectPatterns_Select(
			[lightGroup](PatternData* patternData) {
				lightGroup->AddPattern(patternData);

				Menu::RemoveAllWindows();
				CreateMenu_EditLightGroup_EditPatterns(lightGroup);
			},
			[lightGroup]() {
				Menu::RemoveAllWindows();
				CreateMenu_EditLightGroup_EditPatterns(lightGroup);
			}
			);
		*/
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);

		if (m_OnBack) m_OnBack();
	};
}

void WindowSelectPattern::CreateAddPattern() {
	auto window = Menu::AddWindow("Add pattern");

	//

	static Pattern* pattern;
	pattern = Patterns::m_Patterns[0];

	static std::string patternName;
	patternName = pattern->name;

	auto buttonObjName = window->AddButton("Select pattern");
	buttonObjName->AddTextStr(&patternName, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window]() {

		auto window2 = Menu::AddWindow("", "Select pattern");
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

	auto time = window->AddNumberRange("Time", &timeVal, 1, 999999);
	time->m_OnClick = []() mutable {
		TextEditor::Open("Edit Time", true, &timeVal);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	//

	auto add = window->AddButton("Add");
	add->m_OnClick = [window]() {
		if (m_OnAddPatternCycleStep) m_OnAddPatternCycleStep(pattern, timeVal);

		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto cancel = window->AddButton("Cancel");
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

	auto buttonObjName = window->AddButton("Select pattern");
	buttonObjName->AddTextStr(&patternName, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window, patternCycleStep]() {

		auto window2 = Menu::AddWindow("", "Select pattern");
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

	auto time = window->AddNumberRange("Time", &patternCycleStep->duration, 1, 999999);
	time->m_OnClick = [patternCycleStep]() mutable {
		TextEditor::Open("Edit Time", false, &patternCycleStep->duration);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto buttonDelete = window->AddButton("Delete");
	buttonDelete->m_OnClick = [window, patternCycleStep]() {
		if (m_OnDeletePatternCycleStep) m_OnDeletePatternCycleStep(patternCycleStep);
		
		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto back = window->AddButton("Back");
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreatePatterns();
	};
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