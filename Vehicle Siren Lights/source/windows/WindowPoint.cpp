#include "WindowPoint.h"
#include "WindowLightGroup.h"
#include "WindowMain.h"
#include "WindowShadow.h"
#include "WindowRotateObject.h"

#include "../VehicleDummy.h"
#include "../Vehicles.h"

#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"

#include "../localization/Localization.h"

Point* WindowPoint::m_Point = nullptr;

void WindowPoint::Create() {
	//Vehicles::m_DrawVehiclePoints = true;

	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name + " > Points");

	int i = 0;
	for (auto point : lightGroup->points)
	{
		auto buttonEditPoint = window->AddButton(Localization::GetLineFormatted("edit_point", point->name, (i + 1)));
		buttonEditPoint->AddColorIndicator(&point->color, CVector2D(20, 0));
		buttonEditPoint->m_OnClick = [window, point]() {
			m_Point = point;

			Menu::RemoveWindow(window);
			CreateEditPoint();
		};

		i++;
	}

	auto buttonAddPoint = window->AddButton(Localization::GetLine("add_point"));
	buttonAddPoint->m_OnClick = [window, lightGroup]() {

		auto point = lightGroup->AddPoint(CVector(0, 0, 0), CRGBA(255, 0, 0));

		Menu::RemoveWindow(window);
		Create();
	};


	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateEditLightGroup();

		//Vehicles::m_DrawVehiclePoints = false;
	};
}

void WindowPoint::CreateEditPoint() {
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto point = m_Point;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name + " > Points");

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&point->name, CVector2D(10, 0));
	buttonName->m_OnClick = [point]() {
		TextEditor::Open("Edit name", true, &point->name);
	};

	auto positionButton = window->AddButton(Localization::GetLine("edit_position"));
	positionButton->m_OnClick = [point]() {
		PositionEditor::Toggle(&point->position);
	};

	static int tmpVal;
	tmpVal = (int)point->sirenPosition;

	if (!lightGroup->IsLightbar())
	{
		auto optionsSirenPos = window->AddOptions(Localization::GetLine("siren_position"), &tmpVal);
		optionsSirenPos->AddOption(Localization::GetLine("left"), (int)eSirenPosition::LEFT);
		optionsSirenPos->AddOption(Localization::GetLine("middle"), (int)eSirenPosition::MIDDLE);
		optionsSirenPos->AddOption(Localization::GetLine("right"), (int)eSirenPosition::RIGHT);
		optionsSirenPos->m_OnChange = [point, optionsSirenPos]() {
			point->sirenPosition = (eSirenPosition)tmpVal;
		};
	}

	auto buttonColor = window->AddButton(Localization::GetLine("color"));
	buttonColor->AddColorIndicator(&point->color, CVector2D(20, 0));
	buttonColor->m_OnClick = [window, point]() {
		Menu::CreateColorPickerWindow(&point->color, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	if (!lightGroup->IsLightbar())
	{
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
	

		auto buttonDisabledColor = window->AddButton(Localization::GetLine("disabled_object_color"));
		buttonDisabledColor->AddColorIndicator(&point->disabledColor, CVector2D(20, 0));
		buttonDisabledColor->m_OnClick = [window, point]() {
			Menu::CreateColorPickerWindow(&point->disabledColor, [window]() {
				Menu::m_ActiveWindow = window;
			});
		};
	}

	auto buttonShadow = window->AddButton(Localization::GetLine("edit_shadow"));
	buttonShadow->m_OnClick = [point, window]() {
		Menu::RemoveWindow(window);
		WindowShadow::m_LightGroupShadow = &point->shadow;
		WindowShadow::Create();
	};

	if (!lightGroup->IsLightbar())
	{
		auto buttonRotateObject = window->AddButton("Rotate Object");
		buttonRotateObject->m_OnClick = [point, window]() {
			Menu::RemoveWindow(window);
			WindowRotateObject::m_LightGroupRotateObject = &point->rotateObject;
			WindowRotateObject::Create();
		};
	}

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup, point]() {
		if (lightGroup->points.size() < 2) return;

		lightGroup->RemovePoint(point);

		Menu::RemoveWindow(window);
		Create();
	};
	
	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, lightGroup, point]() {
		auto newPoint = lightGroup->AddPoint(CVector(), CRGBA());
		newPoint->FromJSON(point->ToJSON());

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		Create();
	};
}

