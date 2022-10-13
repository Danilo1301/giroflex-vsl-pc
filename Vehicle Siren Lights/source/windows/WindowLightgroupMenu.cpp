#include "WindowLightgroupMenu.h"

#include "../Vehicles.h"

#include "../menu/Menu.h"
#include "../localization/Localization.h"

#include "../TestHelper.h"

std::vector<LightGroup*> WindowLightgroupMenu::m_LightGroups;
Window* WindowLightgroupMenu::m_Window = NULL;

void WindowLightgroupMenu::Create()
{
	if (!Menu::m_IsOpen) Menu::SetOpen(true);

	auto vehicle = Vehicles::GetVehicle(FindPlayerVehicle(0, false));
	auto firstLightgroup = m_LightGroups[0];
	auto firstLightgroupData = vehicle->m_LightGroupData[firstLightgroup];

	TestHelper::AddLine("[WindowLightgroupMenu] Open for " + std::to_string(firstLightgroup->modelId));

	auto window = m_Window = Menu::AddWindow();
	window->AddItem("Lightgroups: " + std::to_string(m_LightGroups.size()));

	auto changePattern = window->AddButton(Localization::GetLine("lightgroup_change_pattern"));
	changePattern->m_OnClick = [vehicle, firstLightgroup]() {
		for (auto lightGroup : WindowLightgroupMenu::m_LightGroups)
		{
			vehicle->GoToNextLightGroupPattern(lightGroup);
		}
	};

	auto pauseUnpause = window->AddButton(firstLightgroupData->paused ? Localization::GetLine("lightgroup_resume") : Localization::GetLine("lightgroup_pause"));
	pauseUnpause->m_OnClick = [vehicle, firstLightgroupData]() {

		bool s = firstLightgroupData->paused;
		for (auto lightGroup : WindowLightgroupMenu::m_LightGroups)
		{
			vehicle->m_LightGroupData[lightGroup]->paused = !s;
		}
		Reopen();
	};

	auto turnOnOff = window->AddButton(firstLightgroupData->lightsOn ? Localization::GetLine("lightgroup_turn_off") : Localization::GetLine("lightgroup_turn_on"));
	turnOnOff->m_OnClick = [vehicle, firstLightgroupData]() {

		bool s = firstLightgroupData->lightsOn;
		for (auto lightGroup : WindowLightgroupMenu::m_LightGroups)
		{
			vehicle->m_LightGroupData[lightGroup]->lightsOn = !s;
		}
		Reopen();
	};

	auto changeMode = window->AddButton(Localization::GetLine("pattern_mode") + Localization::GetLine(firstLightgroupData->autoChangePattern ? "pattern_mode_auto" : "pattern_mode_manual"));
	changeMode->m_OnClick = [vehicle, firstLightgroupData]() {

		bool s = firstLightgroupData->autoChangePattern;
		for (auto lightGroup : WindowLightgroupMenu::m_LightGroups)
		{
			vehicle->m_LightGroupData[lightGroup]->autoChangePattern = !s;
		}
		Reopen();
	};

	window->m_Position.x = 1024.0f / 2 - window->m_Size.x / 2;
	window->m_Position.y = 550.0f;
}

void WindowLightgroupMenu::Reopen()
{
	if (!m_Window) return;

	int index = m_Window->m_SelectedIndex;

	Menu::RemoveWindow(m_Window);
	Create();

	m_Window->m_SelectedIndex = index;
}

void WindowLightgroupMenu::Check()
{
	if (m_Window)
	{
		if (m_LightGroups[0]->keybindMenu.CheckKeybind())
		{
			Menu::RemoveAllWindows();
			Menu::SetOpen(false);
			m_Window = NULL;
			m_LightGroups.clear();
		}
		return;
	}

	if (Menu::m_IsOpen) return;

	auto vehicle = FindPlayerVehicle(0, false);

	if (!Vehicles::HasVehicle(vehicle))
		return;

	for (auto pair : Vehicles::GetVehicle(vehicle)->m_LightGroupData)
	{
		auto lightGroup = pair.first;

		if (!lightGroup->keybindMenu.CheckKeybind())
			continue;

		m_LightGroups.push_back(lightGroup);
	}

	if (m_LightGroups.size() == 0) return;

	Create();
}