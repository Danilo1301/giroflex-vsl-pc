#include "WindowLightGroup.h"
#include "WindowMain.h"
#include "WindowPoint.h"
#include "WindowSelectPattern.h"

#include "../Mod.h"
#include "../Patterns.h"
#include "../Vehicles.h"
#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"
#include "../menu/KeySelector.h"

#include "../localization/Localization.h"

LightGroup* WindowLightGroup::m_LightGroup = nullptr;

static int amountLights;
static float distance;
static float curve;
static bool shadow;

void WindowLightGroup::CreateLightGroups() {
	auto veh = WindowMain::m_Vehicle;
	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups");

	auto buttonAddLightGroupSiren = window->AddButton(Localization::GetLine("add_light_group"));
	buttonAddLightGroupSiren->m_OnClick = [veh, window]() mutable {

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex);
		lightGroup->position = CVector(0, 0, 2);
		lightGroup->AddPoint(CVector(-0.3f, 0, 0), CRGBA(255, 0, 0), eSirenPosition::LEFT);
		//lightGroup->AddPoint(CVector(0, 0, 0), CRGBA(255, 255, 255), eSirenPosition::MIDDLE);
		lightGroup->AddPoint(CVector(0.3f, 0, 0), CRGBA(0, 0, 255), eSirenPosition::RIGHT);

		for (auto pattern : Patterns::m_Patterns)
		{
			if (pattern->fileName.find("default") != std::string::npos)
			{
				lightGroup->AddPatternCycleStep(pattern, 10000);
			}
		}

		Menu::RemoveWindow(window);
		CreateLightGroups();
	};

	auto buttonAddLightbar = window->AddButton(Localization::GetLine("add_lightbar"));
	buttonAddLightbar->m_OnClick = [veh, window]() mutable {

		auto lightGroup = LightGroups::CreateLightbarLightGroup(veh->m_nModelIndex);
		lightGroup->name = "Lightbar";
		lightGroup->position = CVector(0, 0, 2);

		for (auto pattern : Patterns::m_Patterns)
		{
			if (pattern->fileName.find("lightbar-7lights") != std::string::npos)
			{
				lightGroup->AddPatternCycleStep(pattern, 10000);
			}
		}

		m_LightGroup = lightGroup;
		Menu::RemoveWindow(window);
		Vehicles::m_DrawVehiclePoints = true;

		amountLights = 11;
		distance = 0.1f;
		curve = 0.0f;
		shadow = false;
		CreateSetupLightbar();
	};


	int lightGroupsNum = 0;
	if (LightGroups::HasLightGroups(veh->m_nModelIndex)) lightGroupsNum = LightGroups::GetLightGroups(veh->m_nModelIndex).size();
	window->AddItem("---------- " + std::to_string(lightGroupsNum) + " LightGroups ----------");

	if (LightGroups::HasLightGroups(veh->m_nModelIndex)) {
		auto lightGroups = LightGroups::GetLightGroups(veh->m_nModelIndex);

		for (auto lightGroup : lightGroups)
		{
			auto button1 = window->AddButton(Localization::GetLineFormatted("edit_light_group", lightGroup->name));
			button1->m_OnClick = [window, lightGroup]() mutable {
				m_LightGroup = lightGroup;
				Menu::RemoveWindow(window);
				Vehicles::m_DrawVehiclePoints = true;
				CreateEditLightGroup();
			};
		}

	}

	window->AddItem("--------------------------------------");

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
}

void WindowLightGroup::CreateSetupLightbar() {
	auto veh = WindowMain::m_Vehicle;
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Create lightbar");
	auto lightGroup = m_LightGroup;

	auto positionButton = window->AddButton(Localization::GetLine("edit_position_points"));
	positionButton->m_OnClick = [lightGroup]() {
		PositionEditor::Toggle(&lightGroup->position);
	};

	auto editPatterns = window->AddButton(Localization::GetLineFormatted("edit_patterns", lightGroup->patternCycleSteps.size()));
	editPatterns->m_OnClick = [window, lightGroup]() mutable {
		Menu::RemoveWindow(window);

		WindowSelectPattern::m_PatternCycleSteps = &lightGroup->patternCycleSteps;
		WindowSelectPattern::m_OnAddPatternCycleStep = [lightGroup](Pattern* pattern, int time) {
			Vehicles::RemoveAllVehicles();
			lightGroup->AddPatternCycleStep(pattern, time);
			Vehicles::TryAddAllVehicles();
		};
		WindowSelectPattern::m_OnDeletePatternCycleStep = [lightGroup](PatternCycleStep* patternCycleStep) {
			if (lightGroup->patternCycleSteps.size() == 1) {
				Localization::PrintString("error_need_at_least_one", 1000);
			}

			Vehicles::RemoveAllVehicles();
			lightGroup->RemovePatternCycleStep(patternCycleStep);
			Vehicles::TryAddAllVehicles();
		};
		WindowSelectPattern::m_OnBack = []() {
			WindowSelectPattern::Close();
			CreateSetupLightbar();
		};
		WindowSelectPattern::CreatePatterns();
	};

	auto editAmountLights = window->AddNumberRange(Localization::GetLine("edit_lightbar_amount_lights"), &amountLights, 1, 50);
	editAmountLights->m_OnChange = [lightGroup]() {
		lightGroup->RemoveAllPoints();
		lightGroup->UpdateLightbarPoints(amountLights, curve, distance, shadow);
	};

	auto editCurve = window->AddNumberRange(Localization::GetLine("edit_lightbar_curve"), &curve, 0.0f, 2.0f);
	editCurve->m_AddBy = 0.0005f;
	editCurve->m_OnChange = [lightGroup]() {
		lightGroup->RemoveAllPoints();
		lightGroup->UpdateLightbarPoints(amountLights, curve, distance, shadow);
	};

	auto editDistance = window->AddNumberRange(Localization::GetLine("edit_lightbar_distance_lights"), &distance, 0.01f, 10.0f);
	editDistance->m_AddBy = 0.0005f;
	editDistance->m_OnChange = [lightGroup]() {
		lightGroup->RemoveAllPoints();
		lightGroup->UpdateLightbarPoints(amountLights, curve, distance, shadow);
	};

	auto buttonLedPrefix = window->AddButton(Localization::GetLine("edit_led_prefix"));
	buttonLedPrefix->AddTextStr(&lightGroup->lightbarSettings.object_prefix, CVector2D(10, 0));
	buttonLedPrefix->m_OnClick = [lightGroup]() {
		TextEditor::Open(Localization::GetLine("edit_led_prefix"), false, &lightGroup->lightbarSettings.object_prefix);
	};

	auto size = window->AddNumberRange(Localization::GetLine("size"), &lightGroup->size, 0.0f, 10.0f);

	auto checkboxShadow = window->AddCheckBox(Localization::GetLine("use_shadow"), &shadow);
	checkboxShadow->m_OnChange = [lightGroup]() {
		lightGroup->UpdateLightbarPoints(amountLights, curve, distance, shadow);
	};

	auto buttonBack = window->AddButton(Localization::GetLine("next"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateEditLightGroup();
		Vehicles::m_DrawVehiclePoints = false;
	};
}

void WindowLightGroup::CreateEditLightGroup() {
	auto lightGroup = m_LightGroup;

	bool isLightbar = lightGroup->lightbarSettings.isLightbar;
	
	if (lightGroup->patternCycleSteps.size() == 0)
	{
		CMessages::AddMessageJumpQ("WARNING: This light group has 0 patterns set! It won't be visible", 2000, 0, false);
	}
	/*
	if (lightGroup->isLightbar)
	{
		CreateEditLightGroup_Lightbar();
		return;
	}
	*/

	CVehicle* veh = WindowMain::m_Vehicle;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name);

	auto checkBoxFreezeLights = window->AddCheckBox(Localization::GetLine("freeze_lights"), &Vehicle::m_FreezeLights);

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&lightGroup->name, CVector2D(10, 0));
	buttonName->m_OnClick = [lightGroup]() {
		TextEditor::Open(Localization::GetLine("edit_name"), true, &lightGroup->name);
	};

	if (isLightbar)
	{
		auto buttonLedPrefix = window->AddButton(Localization::GetLine("edit_led_prefix"));
		buttonLedPrefix->AddTextStr(&lightGroup->lightbarSettings.object_prefix, CVector2D(10, 0));
		buttonLedPrefix->m_OnClick = [lightGroup]() {
			TextEditor::Open(Localization::GetLine("edit_led_prefix"), false, &lightGroup->lightbarSettings.object_prefix);
		};

		auto buttonLedOnColor = window->AddButton(Localization::GetLine("led_on_color"));
		buttonLedOnColor->AddColorIndicator(&lightGroup->lightbarSettings.ledOnColor, CVector2D(20, 0));
		buttonLedOnColor->m_OnClick = [window, lightGroup]() {
			Menu::CreateColorPickerWindow(&lightGroup->lightbarSettings.ledOnColor, [window]() {
				Menu::m_ActiveWindow = window;
			});
		};

		auto buttonLedOffColor = window->AddButton(Localization::GetLine("led_off_color"));
		buttonLedOffColor->AddColorIndicator(&lightGroup->lightbarSettings.ledOffColor, CVector2D(20, 0));
		buttonLedOffColor->m_OnClick = [window, lightGroup]() {
			Menu::CreateColorPickerWindow(&lightGroup->lightbarSettings.ledOffColor, [window]() {
				Menu::m_ActiveWindow = window;
			});
		};
	}

	auto buttonEditPoints = window->AddButton(Localization::GetLineFormatted("edit_points", lightGroup->points.size()));
	buttonEditPoints->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPoint::Create();
	};

	auto editPatterns = window->AddButton(Localization::GetLineFormatted("edit_patterns", lightGroup->patternCycleSteps.size()));
	editPatterns->m_OnClick = [window, lightGroup]() mutable {
		Menu::RemoveWindow(window);

		WindowSelectPattern::m_PatternCycleSteps = &lightGroup->patternCycleSteps;
		WindowSelectPattern::m_OnAddPatternCycleStep = [lightGroup](Pattern* pattern, int time) {
			Vehicles::RemoveAllVehicles();
			lightGroup->AddPatternCycleStep(pattern, time);
			Vehicles::TryAddAllVehicles();
		};
		WindowSelectPattern::m_OnDeletePatternCycleStep = [lightGroup](PatternCycleStep* patternCycleStep) {
			if (lightGroup->patternCycleSteps.size() == 1) {
				Localization::PrintString("error_need_at_least_one", 1000);
			}

			Vehicles::RemoveAllVehicles();
			lightGroup->RemovePatternCycleStep(patternCycleStep);
			Vehicles::TryAddAllVehicles();
		};
		WindowSelectPattern::m_OnBack = []() {
			WindowSelectPattern::Close();
			Vehicles::m_DrawVehiclePoints = true;
			CreateEditLightGroup();
		};
		WindowSelectPattern::CreatePatterns();
	};

	auto positionButton = window->AddButton(Localization::GetLine("edit_position_points"));
	positionButton->m_OnClick = [lightGroup]() {
		PositionEditor::Toggle(&lightGroup->position);
	};

	/*
	auto amountLights = window->AddNumberRange("Amount of lights", &lightGroup->amount, 1, 999);

	auto offset = window->AddNumberRange("Offset", &lightGroup->offsetBy, -9999999, 9999999);
	offset->m_HoldToChange = true;

	auto offsetId = window->AddNumberRange("Offset ID", &lightGroup->offsetId, -9999999, 9999999);
	offsetId->m_HoldToChange = true;
	*/

	auto size = window->AddNumberRange(Localization::GetLine("size"), &lightGroup->size, 0.0f, 10.0f);

	auto nearClip = window->AddNumberRange(Localization::GetLine("near_clip"), &lightGroup->nearClip, 0.0f, 10.0f);

	auto checkBoxReflect = window->AddCheckBox(Localization::GetLine("reflect"), &lightGroup->reflect);

	auto reflectionDistance = window->AddNumberRange(Localization::GetLine("reflection_distance"), &lightGroup->reflectionDistance, 0.0f, 50.0f);
	reflectionDistance->m_HoldToChange = true;
	reflectionDistance->m_AddBy = 0.1f;

	//auto checkBoxUsePatternColors = window->AddCheckBox(Localization::GetLine("use_pattern_colors"), &lightGroup->usePatternColors);

	auto offsetId = window->AddNumberRange("Light ID offset", &lightGroup->offsetId, -999999, 999999);
	offsetId->m_HoldToChange = true;

	//auto checkBoxCReflect = window->AddCheckBox("Corona reflection", &lightGroup->cReflect);

	static int directionVal;
	directionVal = (int)lightGroup->direction;
	auto optionsDirection = window->AddOptions(Localization::GetLine("light_direction"), &directionVal);
	optionsDirection->AddOption(Localization::GetLine("light_direction_back"), (int)eSirenDirection::BACK);
	optionsDirection->AddOption(Localization::GetLine("light_direction_both"), (int)eSirenDirection::BOTH);
	optionsDirection->AddOption(Localization::GetLine("light_direction_front"), (int)eSirenDirection::FRONT);
	optionsDirection->m_OnChange = [lightGroup]() {
		lightGroup->direction = (eSirenDirection)directionVal;
	};


	static int typeVal;
	typeVal = (int)lightGroup->type;
	auto optionsType = window->AddOptions(Localization::GetLine("corona_type"), &typeVal);
	optionsType->AddOption("Circle", (int)eCoronaType::CORONATYPE_CIRCLE);
	optionsType->AddOption("Headlight", (int)eCoronaType::CORONATYPE_HEADLIGHT);
	optionsType->AddOption("Headlight Line", (int)eCoronaType::CORONATYPE_HEADLIGHTLINE);
	optionsType->AddOption("Hex", (int)eCoronaType::CORONATYPE_HEX);
	optionsType->AddOption("Moon", (int)eCoronaType::CORONATYPE_MOON);
	optionsType->AddOption("Reflection", (int)eCoronaType::CORONATYPE_REFLECTION);
	optionsType->AddOption("Ring", (int)eCoronaType::CORONATYPE_RING);
	optionsType->AddOption("Shinystar", (int)eCoronaType::CORONATYPE_SHINYSTAR);
	optionsType->AddOption("Streak", (int)eCoronaType::CORONATYPE_STREAK);
	optionsType->AddOption("Torus", (int)eCoronaType::CORONATYPE_TORUS);
	optionsType->m_OnChange = [lightGroup]() {
		lightGroup->type = (eCoronaType)typeVal;
	};

	static int flareTypeVal;
	flareTypeVal = (int)lightGroup->flareType;
	auto optionsFlareType = window->AddOptions(Localization::GetLine("flare_type"), &flareTypeVal);
	optionsFlareType->AddOption("None", (int)eCoronaFlareType::FLARETYPE_NONE);
	optionsFlareType->AddOption("Headlights", (int)eCoronaFlareType::FLARETYPE_HEADLIGHTS);
	optionsFlareType->AddOption("Sun", (int)eCoronaFlareType::FLARETYPE_SUN);
	optionsFlareType->m_OnChange = [lightGroup]() {
		lightGroup->flareType = (eCoronaFlareType)flareTypeVal;
	};

	/*
	auto checkBoxLerp = window->AddCheckBox("Lerp color", &lightGroup->lerpColor);

	auto checkBoxSiren = window->AddCheckBox("Turn on siren", &lightGroup->turnOnSiren);


	auto buttonKey = window->AddButtonKey("Key to toggle", lightGroup->keys);
	*/

	auto buttonKeybind = window->AddButtonKey(Localization::GetLine("lightgroup_keybind"), &lightGroup->keybindMenu);
	
	buttonKeybind->m_OnClick = [window, lightGroup]() {
		KeySelector::Toggle(&lightGroup->keybindMenu);
	};

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup]() {
		Menu::CreateConfirmWindow("", "Delete this lightgroup?", "Yes, delete", "No, cancel", [window, lightGroup](){
			Vehicles::RemoveAllVehicles();
			LightGroups::RemoveLightGroup(lightGroup);
			Vehicles::TryAddAllVehicles();

			Menu::RemoveWindow(window);
			CreateLightGroups();
		});
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		m_LightGroup = NULL;
		CreateLightGroups();
	};
}