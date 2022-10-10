#include "WindowMain.h"

#include "../menu/Menu.h"
#include "../Vehicle.h"

#include "../localization/Localization.h"

CVehicle* WindowMain::CurrentVehicle = nullptr;

void WindowMain::CreateMain()
{
	auto window = Menu::AddWindow("Vehicle Siren Lights", "whoa");

	//window->AddItem("haha");

	auto optionsLanguage = window->AddButton(Localization::GetLine("main_language"));
	optionsLanguage->m_OnClick = [window]()
	{
		auto window2 = Menu::AddWindow("NEW", "whoa AGAINE");
		window2->m_Position.x += 200;
		window2->m_Position.y += 200;

		//window2->AddItem("haha");

		for (size_t i = 0; i < 5; i++)
		{
			window2->AddButton("hi");
		}

		//window2->AddItem("haha");
	};

	for (size_t i = 0; i < 20; i++)
	{
		window->AddButton("hi");
	}

	//window->AddItem("haha");

	/*
	auto window = Menu::AddWindow("Vehicle Siren Lights", Localization::GetLine("info_vehicle_id") + " " + std::to_string(CurrentVehicle->m_nModelIndex) + " - v" + Mod::Version);
	
	window->AddItem(Localization::GetLine("info_move_camera"));

	if (Mod::IsSamp) window->AddItem(Localization::GetLine("info_hide_samp_chat"));

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
	*/
}