#include "WindowShadow.h"
#include "WindowLightGroup.h"
#include "WindowPoint.h"
#include "WindowMain.h"

#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"

#include "../localization/Localization.h"

LightGroupShadow* WindowShadow::m_LightGroupShadow = NULL;

void WindowShadow::Create() {
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name + " > Points > Shadow");

	auto buttonEnabled = window->AddCheckBox(Localization::GetLine("enable_shadow"), &m_LightGroupShadow->enabled);

	auto buttonTexture = window->AddNumberRange(Localization::GetLine("texture"), &m_LightGroupShadow->textureIndex, 0, LightGroupShadows::NUM_OF_SHADOWS - 1);

	auto positionButton = window->AddButton(Localization::GetLine("edit_position"));
	positionButton->m_OnClick = []() {
		PositionEditor::Toggle(&m_LightGroupShadow->position);
	};

	auto buttonWidth = window->AddNumberRange(Localization::GetLine("width"), &m_LightGroupShadow->width, 0.0f, 100.0f);

	auto buttonHeight = window->AddNumberRange(Localization::GetLine("height"), &m_LightGroupShadow->height, 0.0f, 100.0f);

	auto buttonAngle = window->AddNumberRange(Localization::GetLine("angle"), &m_LightGroupShadow->angle, 0.0f, 360.0f);
	buttonAngle->m_AddBy = 0.5f;

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPoint::CreateEditPoint();
	};
}