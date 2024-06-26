#include "WindowSettings.h"
#include "WindowMain.h"

#include "../Vehicle.h"

#include "../menu/Menu.h"
#include "../localization/Localization.h"

void WindowSettings::CreateSettings() {
	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, Localization::GetLine("main_settings"));

	auto matAmbient = window->AddNumberRange(Localization::GetLine("settings_material_ambient"), &Vehicle::m_MatAmbient, 0.0f, 20.0f);
	matAmbient->m_AddBy = 0.1f;

	auto lightIdOffset = window->AddNumberRange("Light ID offset", &Vehicle::m_LightIdOffset, 0, 999999);
	lightIdOffset->m_HoldToChange = true;

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
	window->SetBackButton(buttonBack);
}