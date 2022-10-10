#include "Mod.h"

#include "windows/WindowMain.h"
//#include "windows/WindowTest.h"

#include "Vehicles.h"
#include "Config.h"
#include "Keybinds.h"

#include "CVisibilityPlugins.h"

std::string Mod::Version = "2.0";
bool Mod::IsSamp = false;
bool Mod::DebugEnabled = false;

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
	Log::file << "ToggleLights = " << Keybinds::ToggleLights.GetKeybindString() << std::endl;
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
}

void Mod::Draw()
{
	Menu::Draw();
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