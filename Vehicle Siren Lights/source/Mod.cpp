#include "Mod.h"

#include "windows/WindowMain.h"
#include "windows/WindowTest.h"

#include "Patterns.h"
#include "Vehicles.h"
#include "LightGroups.h"
#include "LightGroupShadow.h"
#include "Config.h"

bool Mod::m_DebugEnabled = false;
bool Mod::m_IsSamp = false;
std::string Mod::m_Version = "1.3";

/*

CVehicle* testVehicle = NULL;
#include "VehicleDummy.h"


void TestUpdate() {
	if (Input::GetKeyDown(74)) {
		testVehicle = FindPlayerVehicle(0, false);

		if (testVehicle) {
			auto frames = VehicleDummy::GetFramesOnVehicle(testVehicle);

			for (auto frame : frames) {
				std::string name = GetFrameNodeName(frame);

				auto p = frame->modelling.pos;
				auto parent = RwFrameGetParent(frame);

				Log::file << "---------------" << std::endl;
				Log::file << GetFrameInfo(frame) + "\nchild=" << GetFrameInfo(frame->child) << "\nnext=" << GetFrameInfo(frame->next) << "\nparent=" << GetFrameInfo(parent) << std::endl;
			
				std::vector<RwFrame*> hie;
				RwFrame* f = frame;

				while (f != NULL && f != testVehicle->m_pRwClump->object.parent)
				{
					hie.insert(hie.begin(), f);

					f = RwFrameGetParent(f);
				}

				for (auto tf : hie) {
					std::string tfname = GetFrameNodeName(tf);
					Log::file << tfname << " | ";
				}
				Log::file << std::endl;
			}

			for (auto frame : frames) {
				
				
			}
		}
	}
}



void TestDraw() {
	if (testVehicle) {
		char text[512];
		auto frames = VehicleDummy::GetFramesOnVehicle(testVehicle);

		for (auto frame : frames) {
			if (frame == testVehicle->m_pRwClump->object.parent) continue;


			
			//auto p = frame->modelling.pos;

			//CVector position = m_Vehicle->TransformFromObjectSpace(GetFrameNodePosition(frame));

			//sprintf(text, "%s", GetFrameInfo(frame).c_str());
			//DrawWorldText(text, position);
		}

		DrawWorldText("testVeh", testVehicle->GetPosition());

	}

}
*/

static float float1 = 0.0f;
static float float2 = 7.0f;
static float float3 = 3.5f;
static float float4 = 7.0f;
static float float5 = 0.0f;
static int int1 = 255;

void TestShadowMenu()
{
	auto window = Menu::AddWindow("Vehicle Siren Lights", "test menu");

	window->AddNumberRange("float1", &float1, -9999.0f, 9999.0f);
	window->AddNumberRange("float2", &float2, -9999.0f, 9999.0f);
	window->AddNumberRange("float3", &float3, -9999.0f, 9999.0f);
	window->AddNumberRange("float4", &float4, -9999.0f, 9999.0f);
	auto float5nr = window->AddNumberRange("float5", &float5, -9999.0f, 9999.0f);
	float5nr->m_AddBy = 0.5f;
	auto int1nr = window->AddNumberRange("int1", &int1, 0, 255);
	int1nr->m_HoldToChange = true;

	Menu::SetOpen(true);
}

void Mod::Update() {
	Input::Update();

	//

	if (Input::GetKey(17)) {
		if (Input::GetKeyDown(83)) {
			TestHelper::RunNextStep();
		}
	}

	//

	if (Input::GetKey(17)) {
		if (Input::GetKeyDown(82)) {
			ReloadConfig();
		}
	}

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
	//TestDraw();

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
			Localization::PrintString("error_need_vehicle", 1000);
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

void Mod::ReloadConfig() {
	if (Menu::m_Visible) {
		ToggleMenu();
	}

	CMessages::AddMessageJumpQ("Reloading config...", 1000, 0, false);

	Vehicles::RemoveAllVehicles();
	LightGroups::RemoveAllLightGroups();
	Patterns::RemoveAllPatterns();

	Config::LoadJSON();

	Vehicles::TryAddAllVehicles();

	CMessages::AddMessageJumpQ("Config reloaded", 1000, 0, false);
}

void Mod::DisableOriginalLights() {
	//https://gtaforums.com/topic/757430-block-siren-lights-memory-address-for-it/

	
	//0@ = 0xC3F12C //CPointLight => RGB
	int pointLight = 0xC3F12C;

	//0A8C: write_memory 0@ size 4 value 0.0 virtual_protect 0 // R
	plugin::patch::SetUInt(pointLight, 0);

	//0@ += 4
	pointLight += 4;

	//0A8C: write_memory 0@ size 4 value 0.0 virtual_protect 0  // G
	plugin::patch::SetUInt(pointLight, 0);

	//0@ += 4
	pointLight += 4;

	//0A8C: write_memory 2@ size 4 value 0.0 virtual_protect 0 
	plugin::patch::SetUInt(pointLight, 0);
	



	//NOPs the function that draws the coronnas
	//0A8C: write_memory 0x6ABA60 size 4 value 0x90909090 virtual_protect 0
	plugin::patch::SetUInt(0x6ABA60, 0x90909090);

	//0A8C: write_memory 0x6ABA64 size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x6ABA64, 0x90);



	/*
	TestHelper::AddStep("test1", []() {
		//NOPs the function that checks wether the siren was activated or not
		//0A8C: write_memory 0x6FFDFC size 1 value 0x90 virtual_protect 0
		plugin::patch::SetUChar(0x6FFDFC, 0x90);

		//0A8C: write_memory 0x6FFDFD size 1 value 0x90 virtual_protect 0
		plugin::patch::SetUChar(0x6FFDFD, 0x90);

		//0A8C: write_memory 0x6FFDFE size 1 value 0x90 virtual_protect 0
		plugin::patch::SetUChar(0x6FFDFE, 0x90);
	});
	*/




	//blue
	/*
	//0A8C: write_memory 0x70026C size 4 value 0x90909090 virtual_protect 0
	plugin::patch::SetUInt(0x70026C, 0x90909090);
	//0A8C : write_memory 0x700270 size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x700270, 0x90);
	//0A8C : write_memory 0x700271 size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x700271, 0x90);
	*/
	

	/*
	//0A8C : write_memory 0x700261 size 4 value 0x90909090 virtual_protect 0
	plugin::patch::SetUInt(0x700261, 0x90909090);
	//0A8C : write_memory 0x700265 size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x700265, 0x90);
	//0A8C : write_memory 0x700266 size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x700266, 0x90);
	*/
	
	
	/*
	//0A8C : write_memory 0x700257 size 4 value 0x90909090 virtual_protect 0
	plugin::patch::SetUInt(0x700257, 0x90909090);
	//0A8C : write_memory 0x70025B size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x70025B, 0x90);
	//0A8C : write_memory 0x70025C size 1 value 0x90 virtual_protect 0
	plugin::patch::SetUChar(0x70025C, 0x90);
	*/
	
	

	//NOPs the function that activates the shadow drawing under the vehicle
	//0A8C: write_memory 0x70802D size 4 value 0x90909090 virtual_protect 0
	plugin::patch::SetUInt(0x70802D, 0x90909090);
}

Mod::Mod() {
	Log::Open("Vehicle Siren Lights.log");
	Log::file << "Initialized v" << m_Version << std::endl;

	TestShadowMenu();

	if (GetModuleHandle("SAMP.dll"))
	{
		m_IsSamp = true;
		//Menu::m_DefaultPosition.y += 140.0f;
	}

	DisableOriginalLights();

	Config::LoadJSON();

	if (Patterns::m_Patterns.size() == 0) {
		auto pattern1 = Patterns::CreatePattern("DEFAULT Red 1");
		pattern1->AddStep(1, 0, 0, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 0, 0, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 1, 1, CRGBA(255, 0, 0), 120);
		pattern1->AddStep(0, 1, 0, CRGBA(255, 0, 0), 120);

		auto pattern2 = Patterns::CreatePattern("DEFAULT Red 2");
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

		auto pattern3 = Patterns::CreatePattern("DEFAULT Red 3");
		pattern3->AddStep(1, 0, 0, CRGBA(255, 0, 0), 135);
		pattern3->AddStep(0, 1, 0, CRGBA(255, 0, 0), 135);
		pattern3->AddStep(0, 0, 1, CRGBA(255, 0, 0), 135);
		pattern3->AddStep(0, 1, 0, CRGBA(255, 0, 0), 135);

		auto pattern4 = Patterns::CreatePattern("DEFAULT Red 4");
		pattern4->AddStep(1, 1, 1, CRGBA(255, 0, 0), 60);
		pattern4->AddStep(0, 0, 0, CRGBA(255, 0, 0), 60);

		pattern4->AddStep(1, 1, 1, CRGBA(255, 0, 0), 60);
		pattern4->AddStep(0, 0, 0, CRGBA(255, 0, 0), 60);

		pattern4->AddStep(1, 1, 1, CRGBA(255, 0, 0), 60);
		pattern4->AddStep(0, 0, 0, CRGBA(255, 0, 0), 60);

		pattern4->AddStep(1, 1, 1, CRGBA(255, 0, 0), 60);
		pattern4->AddStep(0, 0, 0, CRGBA(255, 0, 0), 360);

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

	Events::initRwEvent += [] {
		LightGroupShadows::LoadTextures();
	};

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