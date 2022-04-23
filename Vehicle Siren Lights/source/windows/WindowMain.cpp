#include "WindowMain.h"
#include "WindowLightGroup.h"
#include "WindowPattern.h"

#include "../menu/Menu.h"
#include "../Vehicle.h"

#include "../localization/Localization.h"

CVehicle* WindowMain::m_Vehicle = nullptr;

void WindowMain::CreateMain() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", Localization::GetLine("info_vehicle_id") + " " + std::to_string(m_Vehicle->m_nModelIndex));

	window->AddItem(Localization::GetLine("info_move_camera"));

	if (Mod::m_IsSamp) window->AddItem(Localization::GetLine("info_hide_samp_chat"));
	
	auto buttonEditLightGroups = window->AddButton(Localization::GetLine("main_edit_light_groups"));
	buttonEditLightGroups->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateLightGroups();
	};

	auto buttonEditPatterns = window->AddButton(Localization::GetLine("main_edit_patterns"));
	buttonEditPatterns->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPattern::CreatePatterns();
	};

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

	/*
	auto buttonObjName = window->AddButton(Localization::GetLine("select_object"));
	buttonObjName->AddTextStr(&point->object, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window, point, veh]() {
		Vehicles::m_DrawVehicleFrames = true;

		auto window2 = Menu::AddWindow("", Localization::GetLine("select_object"));
		window2->m_Position.x += window2->m_Size.x + 5.0f;

		auto button = window2->AddButton("[None]");
		button->m_OnClick = [window, window2, point]() {
			point->object = "";

			Menu::RemoveWindow(window2);
			Menu::m_ActiveWindow = window;
			Vehicles::m_DrawVehicleFrames = false;
		};

		auto frames = VehicleDummy::GetFramesOnVehicle(veh);
		for (auto frame : frames) {
			if (frame == veh->m_pRwClump->object.parent) continue;

			std::string name = GetFrameNodeName(frame);

			auto button = window2->AddButton(name);
			button->m_OnClick = [window, window2, point, name]() {
				point->object = name;

				Menu::RemoveWindow(window2);
				Menu::m_ActiveWindow = window;
				Vehicles::m_DrawVehicleFrames = false;
			};
		}
	};
	*/

	auto buttonEditMaterial = window->AddButton("Material Settings");
	buttonEditMaterial->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateMaterialProps();
	};
}

void WindowMain::CreateMaterialProps() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Material Settings");

	window->AddNumberRange("Ambient", &Vehicle::m_MatAmbient, 0.0f, 10.0f);
	//window->AddNumberRange("Diffuse", &Vehicle::m_MatDiffuse, 0.0f, 10.0f);
	//window->AddNumberRange("Specular", &Vehicle::m_MatSpecular, 0.0f, 10.0f);

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateMain();
	};
}