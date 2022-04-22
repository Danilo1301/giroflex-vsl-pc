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

LightGroup* WindowLightGroup::m_LightGroup = nullptr;

void WindowLightGroup::CreateLightGroups() {
	auto veh = WindowMain::m_Vehicle;
	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups");

	auto buttonAddLightGroupSiren = window->AddButton("Add light group");
	buttonAddLightGroupSiren->m_OnClick = [veh, window]() mutable {

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex);
		lightGroup->position = CVector(0, 0, 2);
		lightGroup->AddPoint(CVector(-0.3f, 0, 0), CRGBA(255, 0, 0), eSirenPosition::LEFT);
		//lightGroup->AddPoint(CVector(0, 0, 0), CRGBA(255, 255, 255), eSirenPosition::MIDDLE);
		lightGroup->AddPoint(CVector(0.3f, 0, 0), CRGBA(0, 0, 255), eSirenPosition::RIGHT);

		for (size_t i = 0; i < 2; i++)
		{
			if (i <= Patterns::m_Patterns.size() - 1) {
				lightGroup->AddPatternCycleStep(Patterns::m_Patterns[i], 5000);
			}
		}

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
			auto button1 = window->AddButton("[ Edit '" + lightGroup->name + "' ]");
			button1->m_OnClick = [window, lightGroup]() mutable {
				m_LightGroup = lightGroup;
				Menu::RemoveWindow(window);
				CreateEditLightGroup();
			};
		}

	}

	window->AddItem("--------------------------------------");

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
}

void WindowLightGroup::CreateEditLightGroup() {
	auto lightGroup = m_LightGroup;
	CVehicle* veh = WindowMain::m_Vehicle;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name);

	auto checkBoxFreezeLights = window->AddCheckBox("Freeze lights", &Vehicle::m_FreezeLights);

	auto buttonName = window->AddButton("Edit name");
	buttonName->AddTextStr(&lightGroup->name, CVector2D(10, 0));
	buttonName->m_OnClick = [lightGroup]() {
		TextEditor::Open("Edit name", true, &lightGroup->name);
	};

	auto buttonEditPoints = window->AddButton("Edit points (" + std::to_string(lightGroup->points.size()) + " points)");
	buttonEditPoints->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPoint::CreatePoints();
	};

	
	auto editPatterns = window->AddButton("Edit patterns (" + std::to_string(lightGroup->patternCycleSteps.size()) + " patterns)");
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
				CMessages::AddMessageJumpQ("NO", 1000, 0, false);
				return;
			}

			Vehicles::RemoveAllVehicles();
			lightGroup->RemovePatternCycleStep(patternCycleStep);
			Vehicles::TryAddAllVehicles();
		};
		WindowSelectPattern::m_OnBack = []() {
			WindowSelectPattern::Close();
			CreateEditLightGroup();
		};
		WindowSelectPattern::CreatePatterns();
	};
	

	auto positionButton = window->AddButton("Edit position (all points)");
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

	auto size = window->AddNumberRange("Size", &lightGroup->size, 0.0f, 10.0f);

	auto nearClip = window->AddNumberRange("Near clip", &lightGroup->nearClip, 0.0f, 10.0f);

	auto checkBoxReflect = window->AddCheckBox("Reflect", &lightGroup->reflect);

	auto checkBoxUsePatternColors = window->AddCheckBox("Use pattern colors", &lightGroup->usePatternColors);

	auto reflectionDistance = window->AddNumberRange("Reflection distance", &lightGroup->reflectionDistance, 0.0f, 50.0f);
	reflectionDistance->m_HoldToChange = true;
	reflectionDistance->m_AddBy = 0.1f;

	//auto checkBoxCReflect = window->AddCheckBox("Corona reflection", &lightGroup->cReflect);

	static int flareTypeVal;
	flareTypeVal = (int)lightGroup->flareType;
	auto optionsFlareType = window->AddOptions("Flare type", &flareTypeVal);
	optionsFlareType->AddOption("None", (int)eCoronaFlareType::FLARETYPE_NONE);
	optionsFlareType->AddOption("Headlights", (int)eCoronaFlareType::FLARETYPE_HEADLIGHTS);
	optionsFlareType->AddOption("Sun", (int)eCoronaFlareType::FLARETYPE_SUN);
	optionsFlareType->m_OnChange = [lightGroup]() {
		lightGroup->flareType = (eCoronaFlareType)flareTypeVal;
	};

	//0 2 3 4 9
	

	static int typeVal;
	typeVal = (int)lightGroup->type;
	auto optionsType = window->AddOptions("Type", &typeVal);
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

	/*
	auto checkBoxLerp = window->AddCheckBox("Lerp color", &lightGroup->lerpColor);

	auto checkBoxSiren = window->AddCheckBox("Turn on siren", &lightGroup->turnOnSiren);


	auto buttonKey = window->AddButtonKey("Key to toggle", lightGroup->keys);
	*/

	auto buttonDelete = window->AddButton("Delete light group");
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup]() {
		Vehicles::RemoveAllVehicles();
		LightGroups::RemoveLightGroup(lightGroup);
		Vehicles::TryAddAllVehicles();

		Menu::RemoveWindow(window);
		CreateLightGroups();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateLightGroups();
	};
}