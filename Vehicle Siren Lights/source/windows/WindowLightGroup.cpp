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
	auto window = Menu::AddWindow("Title", "LightGroups");

	auto buttonAddLightGroupSiren = window->AddButton("Add light group [siren]");
	buttonAddLightGroupSiren->m_OnClick = [veh, window]() mutable {

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex);
		lightGroup->position = CVector(0, 0, 2);
		lightGroup->AddPoint(CVector(-0.5, 0, 0), eSirenPosition::LEFT);
		lightGroup->AddPoint(CVector(0, 0, 0), eSirenPosition::MIDDLE);
		lightGroup->AddPoint(CVector(0.5, 0, 0), eSirenPosition::RIGHT);

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

	auto window = Menu::AddWindow("Title", "LightGroups > " + lightGroup->name);

	auto editTestInt = window->AddNumberRange("Edit testint", &Mod::testInt, 0, 5000);
	editTestInt->m_HoldToChange = true;
	//editTestInt->AddTextInt(&testInt, CVector2D(10, 0));
	editTestInt->m_OnClick = []() mutable {
		TextEditor::Open("testInt", true, &Mod::testInt);
	};

	auto buttonName = window->AddButton("Edit name");
	buttonName->AddTextStr(&lightGroup->name, CVector2D(10, 0));
	buttonName->m_OnClick = [lightGroup]() mutable {
		TextEditor::m_OnConfirm = []() {

		};
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

	auto checkBoxReflect = window->AddCheckBox("Reflect", &lightGroup->reflect);
	auto reflectionDistance = window->AddNumberRange("Reflection distance", &lightGroup->reflectionDistance, 0.0f, 50.0f);
	reflectionDistance->m_HoldToChange = true;
	reflectionDistance->m_AddBy = 0.1f;

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