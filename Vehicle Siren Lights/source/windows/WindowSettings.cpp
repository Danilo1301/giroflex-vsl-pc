#include "WindowSettings.h"
#include "WindowMain.h"

#include "../Vehicle.h"

#include "../menu/Menu.h"
#include "../localization/Localization.h"

void WindowSettings::CreateSettings() {
	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, Localization::GetLine("main_settings"));

	auto matAmbient = window->AddNumberRange(Localization::GetLine("settings_material_ambient"), &Vehicle::m_MatAmbient, 0.0f, 20.0f);
	matAmbient->m_AddBy = 0.3f;

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::CreateMain();
	};
}