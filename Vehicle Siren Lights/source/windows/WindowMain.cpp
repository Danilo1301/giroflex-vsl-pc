#include "WindowMain.h"
#include "WindowLightGroup.h"
#include "../menu/Menu.h"
//#include "../Mod.h"

CVehicle* WindowMain::m_Vehicle = nullptr;

void WindowMain::CreateMain() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", "ID: " + std::to_string(m_Vehicle->m_nModelIndex));

	auto buttonEditLightGroups = window->AddButton("Edit Light Groups");
	buttonEditLightGroups->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateLightGroups();
	};

	auto buttonEditPatterns = window->AddButton("Edit Patterns");
	buttonEditPatterns->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
	};
}