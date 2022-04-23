#include "WindowPattern.h"
#include "WindowMain.h"

#include "../menu/TextEditor.h"

#include "../Vehicles.h"

#include "../localization/Localization.h"

Pattern* WindowPattern::m_Pattern = nullptr;
PatternStep* WindowPattern::m_PatternStep = nullptr;

static CRGBA disabledColor = CRGBA(0, 0, 0, 255);

void WindowPattern::CreatePatterns() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Patterns");

	int numPatterns = Patterns::m_Patterns.size();

	window->AddItem("---------- " + std::to_string(numPatterns) + " Patterns ----------");

	if (Patterns::m_Patterns.size() > 0) {
		for (auto pattern : Patterns::m_Patterns)
		{
			auto button = window->AddButton(Localization::GetLineFormatted("edit_pattern", pattern->name));
			button->m_OnClick = [window, pattern]() {
				Menu::RemoveWindow(window);
				m_Pattern = pattern;
				CreateEditPattern();
			};
		}
	}

	window->AddItem("--------------------------------------");


	auto addPattern = window->AddButton(Localization::GetLine("add_pattern"));
	addPattern->m_OnClick = [window]() mutable {
		auto pattern = Patterns::CreatePattern("New pattern");
		pattern->AddStep(true, false, false, CRGBA(255, 0, 0), 250);
		pattern->AddStep(false, true, false, CRGBA(255, 0, 0), 250);
		pattern->AddStep(false, false, true, CRGBA(0, 0, 255), 250);
		pattern->AddStep(false, true, false, CRGBA(0, 0, 255), 250);

		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
}

void WindowPattern::CreateEditPattern() {
	auto pattern = m_Pattern;
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Patterns > " + pattern->name);

	int i = 0;
	for (auto step : pattern->steps)
	{
		auto button = window->AddButton(Localization::GetLineFormatted("edit_pattern_step", step->duration));
		button->AddColorIndicator(step->leftState ? &step->leftColor : &disabledColor, CVector2D(70, 0));
		button->AddColorIndicator(step->middleState ? &step->middleColor : &disabledColor, CVector2D(45, 0));
		button->AddColorIndicator(step->rightState ? &step->rightColor : &disabledColor, CVector2D(20, 0));
		button->m_OnClick = [window, step]() mutable {
			m_PatternStep = step;
			Menu::RemoveWindow(window);
			CreateEditStep();
		};
		i++;
	}

	auto buttonAddStep = window->AddButton(Localization::GetLine("add_step"));
	buttonAddStep->m_OnClick = [window, pattern]() {
		Menu::RemoveWindow(window);
		pattern->AddStep(true, false, false, CRGBA(255, 0, 0), 250);
		CreateEditPattern();
	};

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&pattern->name, CVector2D(10, 0));
	buttonName->m_OnClick = [pattern]() {
		TextEditor::Open("Edit name", true, &pattern->name);
	};

	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, pattern]() {

		auto newPattern = Patterns::CreatePattern("");
		newPattern->FromJSON(pattern->ToJSON());
		newPattern->name += " CLONE";

		Menu::RemoveWindow(window);
		CreatePatterns();
	};

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, pattern]() {
		if (Patterns::m_Patterns.size() == 1) {
			Localization::PrintString("error_need_at_least_one", 1000);
			return;
		}

		Menu::RemoveWindow(window);

		Vehicles::RemoveAllVehicles();
		LightGroups::RemovePatternReferences(pattern);
		Patterns::RemovePattern(pattern);
		Vehicles::TryAddAllVehicles();

		m_Pattern = nullptr;

		CreatePatterns();
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreatePatterns();
	};
}

void WindowPattern::CreateEditStep() {
	auto pattern = m_Pattern;
	auto step = m_PatternStep;
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Edit step");

	auto buttonLeftColor = window->AddButton("LEFT color");
	buttonLeftColor->AddColorIndicator(&step->leftColor, CVector2D(20, 0));
	buttonLeftColor->m_OnClick = [step, window]() {
		Menu::SaveIndex();
		Menu::CreateColorPickerWindow(&step->leftColor, [window]() {
			Menu::RemoveWindow(window);
			CreateEditStep();
			Menu::RestoreIndex();
		});
	};

	auto buttonMiddleColor = window->AddButton("MIDDLE color");
	buttonMiddleColor->AddColorIndicator(&step->middleColor, CVector2D(20, 0));
	buttonMiddleColor->m_OnClick = [step, window]() {
		Menu::SaveIndex();
		Menu::CreateColorPickerWindow(&step->middleColor, [window]() {
			Menu::RemoveWindow(window);
			CreateEditStep();
			Menu::RestoreIndex();
		});
	};

	auto buttonRightColor = window->AddButton("RIGHT color");
	buttonRightColor->AddColorIndicator(&step->rightColor, CVector2D(20, 0));
	buttonRightColor->m_OnClick = [step, window]() {
		Menu::SaveIndex();
		Menu::CreateColorPickerWindow(&step->rightColor, [window]() {
			Menu::RemoveWindow(window);
			CreateEditStep();
			Menu::RestoreIndex();
		});
	};

	auto buttonLeftState = window->AddCheckBox("LEFT state", &step->leftState);
	buttonLeftState->m_OnClick = [window]() {
		Menu::SaveIndex();
		Menu::RemoveWindow(window);
		CreateEditStep();
		Menu::RestoreIndex();
	};

	auto buttonMiddleState = window->AddCheckBox("MIDDLE state", &step->middleState);
	buttonMiddleState->m_OnClick = [window]() {
		Menu::SaveIndex();
		Menu::RemoveWindow(window);
		CreateEditStep();
		Menu::RestoreIndex();
	};

	auto buttonRightState = window->AddCheckBox("RIGHT state", &step->rightState);
	buttonRightState->m_OnClick = [window]() {
		Menu::SaveIndex();
		Menu::RemoveWindow(window);
		CreateEditStep();
		Menu::RestoreIndex();
	};

	auto time = window->AddNumberRange(Localization::GetLine("time"), &step->duration, 1, 999999);
	time->m_OnClick = [step]() mutable {
		TextEditor::Open(Localization::GetLine("time"), true, &step->duration);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto button = window->AddButton("Pattern: ");
	button->AddColorIndicator(step->leftState ? &step->leftColor : &disabledColor, CVector2D(70, 0));
	button->AddColorIndicator(step->middleState ? &step->middleColor : &disabledColor, CVector2D(45, 0));
	button->AddColorIndicator(step->rightState ? &step->rightColor : &disabledColor, CVector2D(20, 0));

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, pattern, step]() {
		if (pattern->steps.size() == 1) {
			Localization::PrintString("error_need_at_least_one", 1000);
			return;
		}

		Menu::RemoveWindow(window);

		Vehicles::RemoveAllVehicles();
		pattern->RemoveStep(step);
		Vehicles::TryAddAllVehicles();

		CreateEditPattern();
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateEditPattern();
	};
}