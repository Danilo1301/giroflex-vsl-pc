#include "WindowLightGroup.h"

#include "WindowMain.h"
#include "WindowPoint.h"
#include "WindowSelectPattern.h"
#include "WindowLightbarLed.h"

#include "../Mod.h"
#include "../Patterns.h"
#include "../Vehicles.h"
#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"
#include "../menu/KeySelector.h"

#include "../localization/Localization.h"

LightGroup* WindowLightGroup::m_LightGroup = nullptr;

void WindowLightGroup::CreateLightGroups() {
	auto veh = WindowMain::m_Vehicle;
	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "LightGroups");

	auto buttonAddLightGroup = window->AddButton(Localization::GetLine("add_light_group"));
	buttonAddLightGroup->m_OnClick = [veh, window]() mutable {
		Vehicles::RemoveAllVehicles();

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex, "");
		lightGroup->position = CVector(0, 0, 2);
		lightGroup->AddPoint();
		lightGroup->AddPoint();

		lightGroup->FindNewPatterns();

		Vehicles::TryAddAllVehicles();

		Menu::RemoveWindow(window);
		CreateLightGroups();
	};

	auto buttonAddPositionLightGroup = window->AddButton(Localization::GetLine("add_position_light_group"));
	buttonAddPositionLightGroup->m_OnClick = [veh, window]() mutable {
		Vehicles::RemoveAllVehicles();

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex, "");
		lightGroup->isPositionLightGroup = true;
		lightGroup->position = CVector(0, 0, 2);
		lightGroup->AddPoint(eSirenPosition::LEFT);
		lightGroup->AddPoint(eSirenPosition::MIDDLE);
		lightGroup->AddPoint(eSirenPosition::RIGHT);

		lightGroup->FindNewPatterns();

		Vehicles::TryAddAllVehicles();

		Menu::RemoveWindow(window);
		CreateLightGroups();
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

void WindowLightGroup::CreateEditLightGroup() {
	auto lightGroup = m_LightGroup;

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

	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "LightGroups > " + lightGroup->name);

	auto checkBoxFreezeLights = window->AddCheckBox(Localization::GetLine("freeze_lights"), &Vehicle::m_FreezeLights);

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&lightGroup->name, CVector2D(10, 0));
	buttonName->m_OnClick = [lightGroup]() {
		TextEditor::Open(Localization::GetLine("edit_name"), true, &lightGroup->name);
	};

	static int amountOfPointsVal;
	amountOfPointsVal = (int)lightGroup->points.size();
	auto amountOfPoints = window->AddOptions(Localization::GetLine("amount_of_lights"), &amountOfPointsVal);
	for (int i = 0; i <= 11; i++)
	{
		amountOfPoints->AddOption(std::to_string(i) + " " + Localization::GetLine("lights"), i);
	}
	amountOfPoints->m_OnChange = [lightGroup]() {
		Vehicles::RemoveAllVehicles();

		lightGroup->SetNumberOfPoints(amountOfPointsVal);
		lightGroup->FindNewPatterns();
		
		Vehicles::TryAddAllVehicles();
	};

	auto pointsPositionFixed = window->AddCheckBox(Localization::GetLine("points_position_fixed"), &lightGroup->pointsPositionFixed);

	auto size = window->AddNumberRange(Localization::GetLine("size"), &lightGroup->size, 0.0f, 10.0f);

	auto distance = window->AddNumberRange(Localization::GetLine("distance"), &lightGroup->distance, 0.0f, 10.0f);
	distance->m_AddBy = 0.001f;

	auto curve = window->AddNumberRange(Localization::GetLine("curve"), &lightGroup->curve, -1.0f, 1.0f);
	curve->m_AddBy = 0.0001f;

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

	auto buttonColor1 = window->AddButton(Localization::GetLine("color") + " 1");
	buttonColor1->AddColorIndicator(&lightGroup->color1, CVector2D(20, 0));
	buttonColor1->m_OnClick = [window, lightGroup]() {
		Menu::CreateColorPickerWindow(&lightGroup->color1, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonColor2 = window->AddButton(Localization::GetLine("color") + " 2");
	buttonColor2->AddColorIndicator(&lightGroup->color2, CVector2D(20, 0));
	buttonColor2->m_OnClick = [window, lightGroup]() {
		Menu::CreateColorPickerWindow(&lightGroup->color2, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonColor3 = window->AddButton(Localization::GetLine("color") + " 3");
	buttonColor3->AddColorIndicator(&lightGroup->color3, CVector2D(20, 0));
	buttonColor3->m_OnClick = [window, lightGroup]() {
		Menu::CreateColorPickerWindow(&lightGroup->color3, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	if (!lightGroup->isPositionLightGroup)
	{
		auto buttonEditLightbarSettings = window->AddButton(Localization::GetLine("edit_lightbar_settings"));
		buttonEditLightbarSettings->m_OnClick = [window]() {
			Menu::RemoveWindow(window);
			WindowLightbarLed::Create();
		};
	}

	/*
	auto amountLights = window->AddNumberRange("Amount of lights", &lightGroup->amount, 1, 999);

	auto offsetId = window->AddNumberRange("Offset ID", &lightGroup->offsetId, -9999999, 9999999);
	offsetId->m_HoldToChange = true;
	*/

	auto nearClip = window->AddNumberRange(Localization::GetLine("near_clip"), &lightGroup->nearClip, 0.0f, 10.0f);

	static int directionVal;
	directionVal = (int)lightGroup->direction;
	auto optionsDirection = window->AddOptions(Localization::GetLine("light_direction"), &directionVal);
	optionsDirection->AddOption(Localization::GetLine("light_direction_back"), (int)eSirenDirection::BACK);
	optionsDirection->AddOption(Localization::GetLine("light_direction_both"), (int)eSirenDirection::BOTH);
	optionsDirection->AddOption(Localization::GetLine("light_direction_front"), (int)eSirenDirection::FRONT);
	optionsDirection->m_OnChange = [lightGroup]() {
		lightGroup->direction = (eSirenDirection)directionVal;
	};

	auto checkBoxReflect = window->AddCheckBox(Localization::GetLine("reflect"), &lightGroup->reflect);

	auto reflectionIntensity = window->AddNumberRange(Localization::GetLine("reflection_intensity"), &lightGroup->reflectionIntensity, 0.0f, 1.0f);
	reflectionIntensity->m_HoldToChange = true;

	auto reflectionDistance = window->AddNumberRange(Localization::GetLine("reflection_distance"), &lightGroup->reflectionDistance, 0.0f, 50.0f);
	reflectionDistance->m_HoldToChange = true;
	reflectionDistance->m_AddBy = 0.1f;

	//auto checkBoxUsePatternColors = window->AddCheckBox(Localization::GetLine("use_pattern_colors"), &lightGroup->usePatternColors);

	auto patternOffset = window->AddNumberRange("Pattern offset", &lightGroup->patternOffset, 0, 999999);
	patternOffset->m_OnChange = []() {
		Vehicles::RemoveAllVehicles();
		Vehicles::TryAddAllVehicles();
	};

	auto offsetId = window->AddNumberRange("Light ID offset", &lightGroup->offsetId, -999999, 999999);
	offsetId->m_HoldToChange = true;

	//auto checkBoxCReflect = window->AddCheckBox("Corona reflection", &lightGroup->cReflect);

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


	auto flare_distance = window->AddNumberRange(Localization::GetLine("flare_distance"), &lightGroup->flareDistance, 0.0f, 1000.0f);

	auto flare_intensity = window->AddNumberRange(Localization::GetLine("flare_intensity"), &lightGroup->flareIntensity, 0.0f, 1.0f);


	auto useSmallWhiteCorona = window->AddCheckBox(Localization::GetLine("use_small_white_corona"), &lightGroup->useSmallWhiteCorona);

	static int smallWhiteCoronaTypeVal;
	smallWhiteCoronaTypeVal = (int)lightGroup->smallWhiteCoronaType;
	auto options_smallWhiteCoronaType = window->AddOptions(Localization::GetLine("small_white_corona_type"), &smallWhiteCoronaTypeVal);
	options_smallWhiteCoronaType->AddOption("Circle", (int)eCoronaType::CORONATYPE_CIRCLE);
	options_smallWhiteCoronaType->AddOption("Headlight", (int)eCoronaType::CORONATYPE_HEADLIGHT);
	options_smallWhiteCoronaType->AddOption("Headlight Line", (int)eCoronaType::CORONATYPE_HEADLIGHTLINE);
	options_smallWhiteCoronaType->AddOption("Hex", (int)eCoronaType::CORONATYPE_HEX);
	options_smallWhiteCoronaType->AddOption("Moon", (int)eCoronaType::CORONATYPE_MOON);
	options_smallWhiteCoronaType->AddOption("Reflection", (int)eCoronaType::CORONATYPE_REFLECTION);
	options_smallWhiteCoronaType->AddOption("Ring", (int)eCoronaType::CORONATYPE_RING);
	options_smallWhiteCoronaType->AddOption("Shinystar", (int)eCoronaType::CORONATYPE_SHINYSTAR);
	options_smallWhiteCoronaType->AddOption("Streak", (int)eCoronaType::CORONATYPE_STREAK);
	options_smallWhiteCoronaType->AddOption("Torus", (int)eCoronaType::CORONATYPE_TORUS);
	options_smallWhiteCoronaType->m_OnChange = [lightGroup]() {
		lightGroup->smallWhiteCoronaType = (eCoronaType)smallWhiteCoronaTypeVal;
	};

	auto smallWhiteCoronaScale = window->AddNumberRange(Localization::GetLine("small_white_corona_scale"), &lightGroup->smallWhiteCoronaScale, 0.0f, 2.0f);

	

	/*
	auto checkBoxLerp = window->AddCheckBox("Lerp color", &lightGroup->lerpColor);

	auto checkBoxSiren = window->AddCheckBox("Turn on siren", &lightGroup->turnOnSiren);


	auto buttonKey = window->AddButtonKey("Key to toggle", lightGroup->keys);
	*/

	auto buttonKeybind = window->AddButtonKey(Localization::GetLine("lightgroup_keybind"), &lightGroup->keybindMenu);
	
	buttonKeybind->m_OnClick = [window, lightGroup]() {
		KeySelector::Toggle(&lightGroup->keybindMenu);
	};

	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, lightGroup, veh]() {
		Vehicles::RemoveAllVehicles();

		auto newLightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex, "");
		newLightGroup->FromJSON(lightGroup->ToJSON());
		newLightGroup->name += " (copy)";
		newLightGroup->FindNewPatterns();

		Vehicles::TryAddAllVehicles();

		Menu::RemoveWindow(window);
		CreateLightGroups();
	};

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup]() {
		Menu::CreateConfirmWindow("", Localization::GetLine("delete_lightgroup_text"), Localization::GetLine("delete_lightgroup_confirm"), Localization::GetLine("delete_lightgroup_cancel"), [window, lightGroup](){
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