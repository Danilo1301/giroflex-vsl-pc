#include "Mod.h"

#include "windows/WindowMain.h"
//#include "windows/WindowTest.h"

#include "Patterns.h"
#include "LightGroups.h"
#include "Vehicles.h"
#include "Config.h"
#include "Keybinds.h"

#include "CVisibilityPlugins.h"

std::string Mod::Version = "2.0";
bool Mod::IsSamp = false;
bool Mod::DebugEnabled = false;

unsigned int _prevTime = 0;

Mod::Mod()
{
	Log::Open("Vehicle Siren Lights.log");

	if (GetModuleHandle("SAMP.dll"))
	{
		IsSamp = true;
	}

	Events::initRwEvent += Init;
	Events::processScriptsEvent += Update;
	Events::drawingEvent += Draw;

	Events::vehicleSetModelEvent.after += [](CVehicle* vehicle, int modelId) {
		//if (!Vehicles::HasVehicle(vehicle)) Vehicles::AddVehicle(vehicle);
	};
	Events::vehicleDtorEvent.before += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) Vehicles::RemoveVehicle(vehicle);
	};
	Events::vehicleRenderEvent.before += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) Vehicles::m_Vehicles[vehicle]->RenderBefore();
	};
	Events::vehicleRenderEvent.after += [](CVehicle* vehicle) {
		if (Vehicles::HasVehicle(vehicle)) Vehicles::m_Vehicles[vehicle]->RenderAfter();
	};
}

void Mod::Init()
{
	Log::file << "Initialized v" << Version << std::endl;

	DisableOriginalLights();

	Log::file << "OpenMenu = " << Keybinds::OpenMenu.GetKeybindString() << std::endl;
	Log::file << "ToggleDebug = " << Keybinds::ToggleDebug.GetKeybindString() << std::endl;
	Log::file << "EditorSlower = " << Keybinds::EditorSlower.GetKeybindString() << std::endl;
	Log::file << "EditorUpDown = " << Keybinds::EditorUpDown.GetKeybindString() << std::endl;
	Log::file << "ToggleLightsMode = " << Keybinds::ToggleLightsMode.GetKeybindString() << std::endl;

	auto pattern1 = Patterns::CreatePattern("Default 1");
	pattern1->AddStep(1, 0, 0, 120);
	pattern1->AddStep(0, 1, 0, 120);
	pattern1->AddStep(0, 0, 1, 120);
	pattern1->AddStep(0, 1, 0, 120);

	auto pattern2 = Patterns::CreatePattern("Default 2");
	pattern2->AddStep(1, 1, 1, 135);
	pattern2->AddStep(0, 0, 0, 135);
	pattern2->AddStep(1, 1, 1, 135);

	auto pattern3 = Patterns::CreatePattern("Default 3");
	pattern3->AddStep(1, 0, 1, 135);
	pattern3->AddStep(1, 1, 1, 135);
	pattern3->AddStep(1, 0, 1, 135);

	auto pattern4 = Patterns::CreatePattern("Lightbar 1 [11 lights]");
	pattern4->AddStep({ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }, 100);
	pattern4->AddStep({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, 100);

	auto pattern5 = Patterns::CreatePattern("Lightbar 2 [11 lights]");
	pattern5->AddStep({ 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, 500);
	pattern5->AddStep({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 500);
	pattern5->AddStep({ 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 }, 500);
	pattern5->AddStep({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 500);



	auto lightGroup1 = LightGroups::CreateLightGroup(596);
	lightGroup1->position = CVector(0, 0, 2);
	lightGroup1->AddPoint(CVector(-0.3f, 0, 0), CRGBA(255, 0, 0), eSirenPosition::LEFT);
	auto testPoint = lightGroup1->AddPoint(CVector(0, 0, 0), CRGBA(255, 0, 0), eSirenPosition::MIDDLE);
	//testPoint->object = "giroflex_led_3";
	lightGroup1->AddPoint(CVector(0.3f, 0, 0), CRGBA(255, 0, 0), eSirenPosition::RIGHT);
	lightGroup1->AddPatternCycleStep(pattern1, 3000);
	lightGroup1->AddPatternCycleStep(pattern2, 3000);
	lightGroup1->AddPatternCycleStep(pattern3, 3000);
	lightGroup1->AddPatternCycleStep(pattern4, 3000);
	lightGroup1->AddPatternCycleStep(pattern5, 3000);
	lightGroup1->keybindPause.SetKey("A", KEYBIND_FLAGS::CTRL);
	lightGroup1->keybindChange.SetKey("D", KEYBIND_FLAGS::CTRL);


	auto lightBar1 = LightGroups::CreateLightbarLightGroup(596);
	lightBar1->position = CVector(0, 1.5, 2);
	lightBar1->AddPatternCycleStep(pattern1, 3000);
	lightBar1->AddPatternCycleStep(pattern2, 3000);
	lightBar1->AddPatternCycleStep(pattern3, 3000);
	lightBar1->AddPatternCycleStep(pattern4, 3000);
	lightBar1->AddPatternCycleStep(pattern5, 3000);
	lightBar1->keybindPause.SetKey("A", KEYBIND_FLAGS::CTRL);
	lightBar1->keybindChange.SetKey("D", KEYBIND_FLAGS::CTRL);

}

void Mod::Update()
{
	Input::Update();
	
	if (Input::GetKey(17) && Input::GetKeyDown(83)) //ctrl + s
	{
		TestHelper::RunNextStep();
	}

	if (Keybinds::ToggleDebug.CheckKeybind())  //ctrl + shift + d
	{
		ToggleDebug();
	}

	if (Keybinds::OpenMenu.CheckKeybind())
	{
		ToggleMenu();
	}

	Menu::Update();

	Vehicles::TryAddAllVehicles();

	//TestHelper::AddLine("dt: " + std::to_string(GetDeltaTime()));

	for (auto pair : Vehicles::m_Vehicles) pair.second->Update();

	_prevTime = CTimer::m_snTimeInMilliseconds;
}

void Mod::Draw()
{
	Menu::Draw();
	
	TestHelper::Draw();

	for (auto pair : Vehicles::m_Vehicles) pair.second->Draw();
}

unsigned int Mod::GetDeltaTime()
{
	unsigned int dt = CTimer::m_snTimeInMilliseconds - _prevTime;
	return dt;
}

void Mod::SetPlayerControl(bool enabled)
{
	Command<Commands::SET_PLAYER_CONTROL>(0, enabled);
}

void Mod::ToggleMenu()
{
	bool open = Menu::Toggle();

	if (open)
	{
		WindowMain::CreateMain();
		SetPlayerControl(false);
	}
	else {
		Menu::RemoveAllWindows();
		SetPlayerControl(true);
	}

	CMessages::AddMessageJumpQ("ToggleMenu", 1000, 0, false);
}

void Mod::ToggleDebug()
{
	DebugEnabled = !DebugEnabled;
	CMessages::AddMessageJumpQ(DebugEnabled ? "Debug enabled" : "Debug disabled", 1000, 0, false);
}

void Mod::ReloadConfig()
{
	CMessages::AddMessageJumpQ("ReloadConfig", 1000, 0, false);

	Log::file << "---------------------------------------------" << std::endl;
	Log::file << "[Mod] Reload config" << std::endl;



	Log::file << "---------------------------------------------" << std::endl;
}

void Mod::DisableOriginalLights()
{
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

Mod mod;