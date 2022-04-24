#include "WindowSettings.h"
#include "WindowMain.h"

#include "../Vehicle.h"

void WindowSettings::CreateSettings() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", Localization::GetLine("main_settings"));

	window->AddNumberRange(Localization::GetLine("settings_material_ambient"), &Vehicle::m_MatAmbient, 0.0f, 20.0f);

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
}