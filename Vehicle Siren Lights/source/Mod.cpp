#include "Mod.h"
#include "menu/Menu.h"

#include "windows/WindowMain.h"
#include "windows/WindowTest.h"

#include "Patterns.h"
#include "Vehicles.h"
#include "LightGroups.h"
#include "Config.h"

bool Mod::m_DebugEnabled = true;
bool Mod::m_IsSamp = false;

void Mod::Update() {
	Input::Update();

	//
	if (Input::GetKey(17)) {
		if (Input::GetKeyDown(74)) {
			auto veh = FindPlayerVehicle(0, false);
			if (veh) {
				if (Vehicles::HasVehicle(veh)) {
					auto vehicle = Vehicles::m_Vehicles[veh];
					vehicle->SetAllLightGroupState(!vehicle->m_PrevLightState);
				}
			}
		}
	}
	//

	if (Input::GetKey(17) && Input::GetKey(16)) {
		if (Input::GetKeyDown(68)) {
			m_DebugEnabled = !m_DebugEnabled;
			CMessages::AddMessageJumpQ(m_DebugEnabled ? "Debug enabled" : "Debug disabled", 1000, 0, false);
		}
	}

	if (Input::GetKey(17)) {
		if (Input::GetKeyDown(76)) {
			ToggleMenu();
		}
	}

	if (Menu::m_Visible) {
		if (Input::GetKeyDown(18)) {
			SetPlayerControl(true);
			Menu::m_Hide = true;
		}
		if (Input::GetKeyUp(18)) {
			SetPlayerControl(false);
			Menu::m_Hide = false;
		}
	}
	else {
		if(Vehicles::m_DrawVehicleFrames) Vehicles::m_DrawVehicleFrames = false;
		if(Vehicles::m_DrawVehiclePoints) Vehicles::m_DrawVehiclePoints = false;
	}

	Menu::Update();

	Vehicles::TryAddAllVehicles();

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

		if (m_DebugEnabled) {
			vehicle->DrawDebug();
		}

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

	if (!Menu::m_Visible) {
		WindowMain::m_Vehicle = FindPlayerVehicle(0, false);
		if (!WindowMain::m_Vehicle) {
			CMessages::AddMessageJumpQ("You must be in a vehicle!", 1000, 0, false);
			return;
		}
	}

	Menu::m_Visible = !Menu::m_Visible;
	Menu::m_Hide = false;

	if (Menu::m_Visible) {
		WindowMain::CreateMain();
		//WindowTest::Create();

		SetPlayerControl(false);
	}
	else {
		Menu::RemoveAllWindows();

		SetPlayerControl(true);

		Config::SaveJSON();
	}
}

Mod::Mod() {
	Log::Open();
	Log::file << "Initialized" << std::endl;

	if (GetModuleHandle("SAMP.dll"))
	{
		m_IsSamp = true;
		Menu::m_DefaultPosition.y += 140.0f;
	}

	Config::LoadJSON();

	if (Patterns::m_Patterns.size() == 0) {

		auto pattern1 = Patterns::CreatePattern("Red 1");
		pattern1->AddStep(1, 0, 0, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 0, 0, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 1, 1, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 1, 0, CRGBA(255, 0, 0), 120);

		auto pattern2 = Patterns::CreatePattern("Red 2");
		pattern2->AddStep(0, 1, 1, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		pattern2->AddStep(0, 1, 1, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		pattern2->AddStep(0, 0, 1, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		pattern2->AddStep(1, 1, 0, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		pattern2->AddStep(1, 1, 0, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		pattern2->AddStep(1, 0, 0, CRGBA(255, 0, 0), 70);
		pattern2->AddStep(1, 0, 1, CRGBA(255, 0, 0), 80);

		/*
		auto lightGroup1 = LightGroups::CreateLightGroup(523);
		lightGroup1->position = CVector(0, 0, 2);
		lightGroup1->AddPoint(CVector(-0.3f, 0, 0), eSirenPosition::LEFT);
		lightGroup1->AddPoint(CVector(0, 0, 0), eSirenPosition::MIDDLE);
		lightGroup1->AddPoint(CVector(0.3f, 0, 0), eSirenPosition::RIGHT);

		lightGroup1->AddPatternCycleStep(pattern1, 3000);
		lightGroup1->AddPatternCycleStep(pattern2, 3000);
		*/
	}
	
	Config::SaveJSON();


	Events::processScriptsEvent += []() {
		Update();
	};

	Events::drawingEvent += []() {
		Draw();
	};

	Events::vehicleSetModelEvent.after += [](CVehicle* vehicle, int modelId) {
		/*
		if (!Vehicles::HasVehicle(vehicle)) {
			Vehicles::AddVehicle(vehicle);
		}
		*/
	};

	Events::vehicleDtorEvent.before += [](CVehicle* vehicle) {
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