#include "WindowMain.h"
#include "WindowLightGroup.h"
//#include "WindowPattern.h"
#include "WindowSettings.h"

#include "../Mod.h"

#include "../menu/Menu.h"
#include "../Vehicle.h"

#include "../localization/Localization.h"

CVehicle* WindowMain::m_Vehicle = nullptr;

void WindowMain::CreateMain() {
	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, Localization::GetLine("info_vehicle_id") + " " + std::to_string(m_Vehicle->m_nModelIndex) + " - v" + Mod::m_Version);

	auto buttonEditLightGroups = window->AddButton(Localization::GetLine("main_edit_light_groups"));
	buttonEditLightGroups->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateLightGroups();
	};

	/*
	auto buttonEditPatterns = window->AddButton(Localization::GetLine("main_edit_patterns"));
	buttonEditPatterns->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPattern::CreatePatterns();
	};
	*/

	auto buttonSettings = window->AddButton(Localization::GetLine("main_settings"));
	buttonSettings->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowSettings::CreateSettings();
	};

	window->AddItem(Localization::GetLine("info_move_camera"));

	if (Mod::m_IsSamp) window->AddItem(Localization::GetLine("info_hide_samp_chat"));

	static int languageVal;
	languageVal = 0;

	auto optionsLanguage = window->AddButton(Localization::GetLine("main_language"));
	optionsLanguage->AddTextStr(&Localization::m_CurrentLanguage, CVector2D(10, 0));
	optionsLanguage->m_OnClick = [window]() {

		auto window2 = Menu::AddWindow("", Localization::GetLine("main_language"));
		window2->m_Position.x += window2->m_Size.x + 5.0f;

		for (auto language : Localization::m_AvailableLanguages) {
			auto button = window2->AddButton(language);
			button->m_OnClick = [window, window2, language]() {

				Localization::m_CurrentLanguage = language;

				Menu::RemoveWindow(window2);
				Menu::RemoveWindow(window);
				CreateMain();
			};
		}
	};

}