#include "WindowMain.h"
#include "WindowLightGroup.h"
#include "WindowPattern.h"

#include "../menu/Menu.h"
#include "../Vehicle.h"

CVehicle* WindowMain::m_Vehicle = nullptr;

void WindowMain::CreateMain() {
	auto window = Menu::AddWindow("Vehicle Siren Lights", "Vehicle ID: " + std::to_string(m_Vehicle->m_nModelIndex));

	window->AddItem("- Press [ALT] to move the camera");

	if (Mod::m_IsSamp) window->AddItem("- Press [F7] to hide samp chat");
	
	auto buttonEditLightGroups = window->AddButton("Edit Light Groups");
	buttonEditLightGroups->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateLightGroups();
	};

	auto buttonEditPatterns = window->AddButton("Edit Patterns");
	buttonEditPatterns->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPattern::CreatePatterns();
	};

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

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateMain();
	};
}