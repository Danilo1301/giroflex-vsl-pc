#include "WindowPoint.h"
#include "WindowLightGroup.h"
#include "WindowMain.h"

#include "../VehicleDummy.h"
#include "../Vehicles.h"

#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"

Point* WindowPoint::m_Point = nullptr;

void WindowPoint::CreatePoints() {
	Vehicles::m_DrawVehiclePoints = true;

	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name + " > Points");

	int i = 0;
	for (auto point : lightGroup->points)
	{
		auto buttonEditPoint = window->AddButton("[ Edit point " + std::to_string(i + 1) + " ]");
		buttonEditPoint->m_OnClick = [window, point]() mutable {
			m_Point = point;

			Menu::RemoveWindow(window);
			CreateEditPoint();
		};

		i++;
	}

	auto buttonAddPoint = window->AddButton("Add point");
	buttonAddPoint->m_OnClick = [window, lightGroup]() {

		lightGroup->AddPoint(CVector(0, 0, 0), CRGBA(255, 0, 0));

		Menu::RemoveWindow(window);
		CreatePoints();
	};


	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateEditLightGroup();

		Vehicles::m_DrawVehiclePoints = false;
	};
}

void WindowPoint::CreateEditPoint() {
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;
	auto point = m_Point;

	auto window = Menu::AddWindow("Vehicle Siren Lights", "LightGroups > " + lightGroup->name + " > Points");

	auto positionButton = window->AddButton("Edit position");
	positionButton->m_OnClick = [point]() {
		PositionEditor::Toggle(&point->position);
	};

	static int tmpVal;
	tmpVal = (int)point->sirenPosition;

	auto optionsSirenPos = window->AddOptions("Siren position", &tmpVal);
	optionsSirenPos->AddOption("Left", (int)eSirenPosition::LEFT);
	optionsSirenPos->AddOption("Middle", (int)eSirenPosition::MIDDLE);
	optionsSirenPos->AddOption("Right", (int)eSirenPosition::RIGHT);
	optionsSirenPos->m_OnChange = [point, optionsSirenPos]() {
		point->sirenPosition = (eSirenPosition)tmpVal;
	};

	auto buttonColor = window->AddButton("Color");
	buttonColor->AddColorIndicator(&point->color, CVector2D(20, 0));
	buttonColor->m_OnClick = [window, point]() {
		Menu::CreateColorPickerWindow(&point->color, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonObjName = window->AddButton("Select object");
	buttonObjName->AddTextStr(&point->object, CVector2D(10, 0));
	buttonObjName->m_OnClick = [window, point, veh]() {
		Vehicles::m_DrawVehicleFrames = true;

		auto window2 = Menu::AddWindow("", "Select object");
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

	auto buttonDisabledColor = window->AddButton("Disabled color (for object)");
	buttonDisabledColor->AddColorIndicator(&point->disabledColor, CVector2D(20, 0));
	buttonDisabledColor->m_OnClick = [window, point]() {
		Menu::CreateColorPickerWindow(&point->disabledColor, [window]() {
			Menu::m_ActiveWindow = window;
		});
	};

	auto buttonDelete = window->AddButton("Delete point");
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup, point]() {
		if (lightGroup->points.size() < 2) return;

		lightGroup->RemovePoint(point);

		Menu::RemoveWindow(window);
		CreatePoints();
	};
	
	auto buttonClone = window->AddButton("Clone");
	buttonClone->m_OnClick = [window, lightGroup, point]() {
		auto newPoint = lightGroup->AddPoint(CVector(), CRGBA());
		newPoint->FromJSON(point->ToJSON());

		Menu::RemoveWindow(window);
		CreatePoints();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreatePoints();
	};
}

