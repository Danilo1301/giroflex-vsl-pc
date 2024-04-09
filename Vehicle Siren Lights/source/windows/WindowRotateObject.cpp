#include "WindowRotateObject.h"
#include "WindowMain.h"
#include "WindowLightGroup.h"
#include "WindowPoint.h"

#include "../Vehicles.h"

#include "../localization/Localization.h"
#include "../menu/Menu.h"

LightGroupRotateObject* WindowRotateObject::m_LightGroupRotateObject = NULL;

void WindowRotateObject::Create() {
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow(Menu::m_DefaultWindowTitle, "LightGroups > " + lightGroup->name + " > Points > Rotate Object");

	auto buttonEnabled = window->AddCheckBox("Enable object rotation", &m_LightGroupRotateObject->rotate);

	auto buttonRotateAlways = window->AddCheckBox("Rotate always", &m_LightGroupRotateObject->rotateAlways);

	auto speed = window->AddNumberRange("Speed", &m_LightGroupRotateObject->speed, -50.0f, 50.0f);
	speed->m_AddBy = 0.01f;

	static int axisVal;
	axisVal = (int)m_LightGroupRotateObject->axis;
	auto optionsAxis = window->AddOptions("[-] Axis", &axisVal);
	optionsAxis->AddOption("X", (int)eRotateObjectAxis::X);
	optionsAxis->AddOption("Y", (int)eRotateObjectAxis::Y);
	optionsAxis->AddOption("Z", (int)eRotateObjectAxis::Z);
	optionsAxis->m_OnChange = []() {
		m_LightGroupRotateObject->axis = (eRotateObjectAxis)axisVal;

		Vehicles::GetVehicle(WindowMain::m_Vehicle)->ResetObjectRotation(WindowPoint::m_Point->object);
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPoint::CreateEditPoint();
	};
}