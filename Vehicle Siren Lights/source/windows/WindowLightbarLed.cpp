#include "WindowLightbarLed.h"

#include "WindowLightGroup.h"
#include "WindowMain.h"

#include "../menu/Menu.h"
#include "../menu/TextEditor.h"

#include "../localization/Localization.h"

void WindowLightbarLed::Create()
{
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "LightGroups > " + lightGroup->name + " > Lightbar settings");

	auto useLightbarLeds = window->AddCheckBox(Localization::GetLine("use_lightbar_leds"), &lightGroup->lightbarSettings.useLightbarLeds);

	auto buttonLedPrefix = window->AddButton(Localization::GetLine("edit_led_prefix"));
	buttonLedPrefix->AddTextStr(&lightGroup->lightbarSettings.object_prefix, CVector2D(10, 0));
	buttonLedPrefix->m_OnClick = [lightGroup]() {
		TextEditor::Open(Localization::GetLine("edit_led_prefix"), false, &lightGroup->lightbarSettings.object_prefix);
	};

	auto buttonLedOnColor = window->AddButton(Localization::GetLine("led_on_color"));
	buttonLedOnColor->AddColorIndicator(&lightGroup->lightbarSettings.ledOnColor, CVector2D(20, 0));
	buttonLedOnColor->m_OnClick = [window, lightGroup]() {
		Menu::CreateColorPickerWindow(&lightGroup->lightbarSettings.ledOnColor, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonLedOffColor = window->AddButton(Localization::GetLine("led_off_color"));
	buttonLedOffColor->AddColorIndicator(&lightGroup->lightbarSettings.ledOffColor, CVector2D(20, 0));
	buttonLedOffColor->m_OnClick = [window, lightGroup]() {
		Menu::CreateColorPickerWindow(&lightGroup->lightbarSettings.ledOffColor, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateEditLightGroup();
	};
	window->SetBackButton(buttonBack);
}