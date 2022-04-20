#include "Mod.h"
#include "menu/Menu.h"

#include "windows/WindowMain.h"
#include "windows/WindowTest.h"

#include "Patterns.h"
#include "Vehicles.h"
#include "LightGroups.h"

int Mod::testInt = 500;

void Mod::Update() {
	Input::Update();

	if (Input::GetKeyDown(88)) {
		ToggleMenu();
	}

	if (Menu::m_Visible) {
		if (Input::GetKeyDown(18)) SetPlayerControl(true);
		if (Input::GetKeyUp(18)) SetPlayerControl(false);
	}
	else {
		if(Vehicles::m_DrawVehicleFrames) Vehicles::m_DrawVehicleFrames = false;
		if(Vehicles::m_DrawVehiclePoints) Vehicles::m_DrawVehiclePoints = false;
	}

	Menu::Update();

	for (auto pair : Vehicles::m_Vehicles)
	{
		Vehicle* vehicle = pair.second;
		vehicle->Update();
	}
}

void Mod::Draw() {
	for (auto pair : Vehicles::m_Vehicles)
	{
		Vehicle* vehicle = pair.second;
		vehicle->Draw();
		vehicle->DrawDebug();

		bool isPlayerVeh = vehicle->m_Vehicle == FindPlayerVehicle(0, false);

		if (isPlayerVeh) {
			if (Vehicles::m_DrawVehicleFrames) {
				vehicle->DrawFrames();
			}

			if (Vehicles::m_DrawVehiclePoints) {
				vehicle->DrawPoints();
			}
		}
	}

	Menu::Draw();
}

void Mod::SetPlayerControl(bool enabled) {
	Command<Commands::SET_PLAYER_CONTROL>(0, enabled);
}

void Mod::ToggleMenu() {
	Menu::m_Visible = !Menu::m_Visible;

	if (Menu::m_Visible) {

		WindowMain::m_Vehicle = FindPlayerVehicle(0, false);
		if (!WindowMain::m_Vehicle) return;

		WindowMain::CreateMain();
		//WindowTest::Create();

		SetPlayerControl(false);
	}
	else {
		Menu::RemoveAllWindows();

		SetPlayerControl(true);
	}
}

Mod::Mod() {
	Log::Open();
	Log::file << "Initialized" << std::endl;

	auto pattern1 = Patterns::CreatePattern("red blue");
	pattern1->AddStep(true, false, false, CRGBA(255, 0, 0), 250);
	pattern1->AddStep(false, true, false, CRGBA(255, 0, 0), 250);
	pattern1->AddStep(false, false, true, CRGBA(0, 0, 255), 250);
	pattern1->AddStep(false, true, false, CRGBA(0, 0, 255), 250);

	auto pattern2 = Patterns::CreatePattern("yellow");
	pattern2->AddStep(true, true, false, CRGBA(255, 255, 0), 250);
	pattern2->AddStep(true, false, false, CRGBA(255, 255, 0), 250);
	pattern2->AddStep(false, true, true, CRGBA(255, 255, 0), 250);
	pattern2->AddStep(false, false, true, CRGBA(255, 255, 0), 250);

	auto lightGroup1 = LightGroups::CreateLightGroup(523);
	lightGroup1->position = CVector(0, 0, 2);
	lightGroup1->AddPoint(CVector(-0.3f, 0, 0), eSirenPosition::LEFT);
	lightGroup1->AddPoint(CVector(0, 0, 0), eSirenPosition::MIDDLE);
	lightGroup1->AddPoint(CVector(0.3f, 0, 0), eSirenPosition::RIGHT);

	lightGroup1->AddPatternCycleStep(pattern1, 3000);
	lightGroup1->AddPatternCycleStep(pattern2, 3000);

	Events::processScriptsEvent += []() {
		Update();
	};

	Events::drawingEvent += []() {
		Draw();
	};

	Events::vehicleSetModelEvent.after += [](CVehicle* vehicle, int modelId) {
		if (!Vehicles::HasVehicle(vehicle)) {
			Vehicles::AddVehicle(vehicle);
		}
	};

	Events::vehicleDtorEvent.after += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) {
			Vehicles::RemoveVehicle(vehicle);
		}
	};

	Events::vehicleRenderEvent.before += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) {
			Vehicles::m_Vehicles[vehicle]->RenderBefore();
		}
	};

	Events::vehicleRenderEvent.after += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) {
			Vehicles::m_Vehicles[vehicle]->RenderAfter();
		}
	};
}

Mod mod;