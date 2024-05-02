#include "WindowEditingOptions.h"

#include "WindowLightGroup.h"
#include "WindowMain.h"

#include "../menu/Menu.h"
//#include "../menu/TextEditor.h"

#include "../localization/Localization.h"

bool WindowEditingOptions::ShowCurrentEditingLightGroup = false;
bool WindowEditingOptions::FreezeLights = false;

void WindowEditingOptions::Create()
{
	//CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "LightGroups > " + lightGroup->name + " > Editing options");

	window->AddCheckBox(Localization::GetLine("freeze_lights"), &FreezeLights);

	window->AddCheckBox(Localization::GetLine("show_current_editing_lightgroup"), &ShowCurrentEditingLightGroup);

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateEditLightGroup();
	};
}