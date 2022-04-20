#include "Mod.h"

#include "input/Input.h"

#include "menu/Menu.h"
#include "menu/Item.h"
#include "menu/Button.h"
#include "menu/NumberRange.h"
#include "menu/Options.h"
#include "menu/PositionEditor.h"
#include "menu/TextEditor.h"
#include "menu/CheckBox.h"

#include "Vehicle.h"
#include "Pattern.h"

bool Mod::m_MainMenuVisible = false;
bool Mod::m_ShowDebug = false;
std::string Mod::m_MenuTitle = "Vehicle Siren Lights";

void CreateTestSelectionMenu() {
	auto window = Menu::AddWindow(Mod::m_MenuTitle, "test");
	window->m_Position = CVector2D(120.0f, 120.0f);

	for (int i = 0; i < 30; i++)
	{
		auto button = window->AddButton("Select " + std::to_string(i));
		button->m_OnClick = [i]() mutable {

		};
	}

	Menu::SetOpen(true);
}

void Mod::CreateMenu_Main() {
	CVehicle* veh = FindPlayerVehicle(0, false);

	auto window = Menu::AddWindow(m_MenuTitle, " ");
	window->m_Position = CVector2D(20.0f, 120.0f);
	//CreateMenuTitle(window, "Vehicle ID " + std::to_string(veh->m_nModelIndex));

	auto buttonEditLightGroups = window->AddButton("Edit Light Groups");
	buttonEditLightGroups->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroups();
	};

	auto buttonEditColorTextures = window->AddButton("Edit Color Textures");
	buttonEditColorTextures->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditColorTextures();
	};

	auto buttonEditPatterns = window->AddButton("Edit Patterns");
	buttonEditPatterns->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditPatterns();
	};
}

void Mod::CreateMenu_EditLightGroups() {
	CVehicle* veh = FindPlayerVehicle(0, false);

	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups");
	window->m_Position = CVector2D(20.0f, 120.0f);
	//CreateMenuTitle(window, "Vehicle ID " + std::to_string(veh->m_nModelIndex) + " > LightGroups");

	if (LightGroups::HasLightGroups(veh->m_nModelIndex)) {
		auto lightGroups = LightGroups::GetLightGroups(veh->m_nModelIndex);

		for (auto lightGroup : lightGroups)
		{
			auto button1 = window->AddButton("[ Edit '" + lightGroup->name + "' ]");
			button1->m_OnClick = [lightGroup]() mutable {
				Menu::RemoveAllWindows();
				CreateMenu_EditLightGroup(lightGroup);
			};
		}

		window->AddDivider(5.0f);
	}

	auto buttonAddLightGroupSingle = (Button*)window->AddItem((Item*)new Button());
	buttonAddLightGroupSingle->m_Label = "Add light group (single light)";
	buttonAddLightGroupSingle->m_OnClick = [veh]() mutable {
		auto lightGroup = LightGroups::CreateDefaultLightGroup(veh->m_nModelIndex);
		lightGroup->amount = 1;

		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroups();
	};

	auto buttonAddLightGroupLine = (Button*)window->AddItem((Item*)new Button());
	buttonAddLightGroupLine->m_Label = "Add light group (line)";
	buttonAddLightGroupLine->m_OnClick = [veh]() mutable {
		auto lightGroup = LightGroups::CreateDefaultLightGroup(veh->m_nModelIndex);
		lightGroup->points[0]->m_Position = CVector(1, 0, 0);
		lightGroup->AddPoint({ -1, 0, 0 });

		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroups();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_Main();
	};
}

void Mod::CreateMenu_EditColorTextures() {
	CVehicle* veh = FindPlayerVehicle(0, false);

	auto window = Menu::AddWindow(m_MenuTitle, "ColorTextures");
	window->m_Position = CVector2D(20.0f, 120.0f);
	//CreateMenuTitle(window, "Vehicle ID " + std::to_string(veh->m_nModelIndex) + " > ColorTextures");

	if (ColorTextures::HasColorTextures(veh->m_nModelIndex)) {
		auto colorTextures = ColorTextures::GetColorTextures(veh->m_nModelIndex);

		for (auto colorTexture : colorTextures)
		{
			auto button1 = (Button*)window->AddItem((Item*)new Button());
			button1->m_Label = "[ Edit '" + colorTexture->name + "' ]";
			button1->m_OnClick = [colorTexture]() mutable {
				Menu::RemoveAllWindows();
				CreateMenu_EditColorTexture(colorTexture);

			};
		}

		window->AddDivider(5.0f);
	}

	auto addColorTexture = (Button*)window->AddItem((Item*)new Button());
	addColorTexture->m_Label = "Add color texture";
	addColorTexture->m_OnClick = [veh]() mutable {
		ColorTextures::CreateDefaultColorTexture(veh->m_nModelIndex, "Color Texture", "textureName");

		Menu::RemoveAllWindows();
		CreateMenu_EditColorTextures();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_Main();
	};
}

void Mod::CreateMenu_EditPatterns() {
	auto window = Menu::AddWindow(m_MenuTitle, "Patterns");
	window->m_Position = CVector2D(20.0f, 120.0f);

	if (Patterns::m_Patterns.size() > 0) {
		for (auto pattern : Patterns::m_Patterns)
		{
			auto button = window->AddButton("[ Edit '" + pattern->name + "' ]");
			button->m_OnClick = [pattern]() mutable {
				Menu::RemoveAllWindows();
				CreateMenu_EditPattern(pattern);
			};
		}

		window->AddDivider(5.0f);
	}

	auto addColor = window->AddButton("Add pattern");
	addColor->m_OnClick = []() mutable {
		auto pattern = Patterns::CreateDefaultPattern("Pattern");

		Menu::RemoveAllWindows();
		CreateMenu_EditPatterns();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_Main();
	};
}

void Mod::CreateMenu_EditLightGroup(LightGroup* lightGroup) {
	CVehicle* veh = FindPlayerVehicle(0, false);

	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + lightGroup->name);
	window->m_Position = CVector2D(20.0f, 120.0f);

	auto buttonName = window->AddButton("Edit name (" + lightGroup->name + ")");
	buttonName->m_OnClick = [lightGroup]() mutable {
		TextEditor::Open("Name", &lightGroup->name);
		TextEditor::m_OnConfirm = [lightGroup]() {
			Menu::RemoveAllWindows();
			CreateMenu_EditLightGroup(lightGroup);
		};
	};

	auto buttonEditPoints = window->AddButton("Edit points (" + std::to_string(lightGroup->points.size()) + " points)");
	buttonEditPoints->m_OnClick = [lightGroup]() {
		Menu::RemoveAllWindows();
		Mod::CreateMenu_EditLightGroup_Points(lightGroup);
	};

	auto editPatterns = window->AddButton("Edit patterns (" + std::to_string(lightGroup->patternsData.size()) + " patterns)");
	editPatterns->m_OnClick = [lightGroup]() mutable {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup_EditPatterns(lightGroup);
	};

	auto positionButton = (Button*)window->AddItem((Item*)new Button());
	positionButton->m_Label = "Edit position (all points)";
	positionButton->m_OnClick = [lightGroup]() {
		PositionEditor::Toggle(&lightGroup->position);
	};

	auto amountLights = window->AddNumberRange("Amount of lights", &lightGroup->amount, 1, 999);

	auto offset = window->AddNumberRange("Offset", &lightGroup->offsetBy, -9999999, 9999999);
	offset->m_HoldToChange = true;

	auto offsetId = window->AddNumberRange("Offset ID", &lightGroup->offsetId, -9999999, 9999999);
	offsetId->m_HoldToChange = true;

	auto size = window->AddNumberRange("Size", &lightGroup->size, 0.0f, 10.0f);

	auto checkBoxLerp = window->AddCheckBox("Lerp color", &lightGroup->lerpColor);

	auto checkBoxSiren = window->AddCheckBox("Turn on siren", &lightGroup->turnOnSiren);

	auto checkBoxUseFlash = window->AddCheckBox("Reflect", &lightGroup->useFlash);

	auto buttonKey = window->AddButtonKey("Key to toggle", lightGroup->keys);


	auto buttonDelete = window->AddButton("Delete light group");
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [lightGroup]() {

		Vehicles::RemoveAllVehicles();
		LightGroups::RemoveLightGroup(lightGroup);
		Vehicles::TryAddAllVehicles();

		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroups();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroups();
	};
}

void Mod::CreateMenu_EditLightGroup_EditPatterns(LightGroup* lightGroup) {
	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + lightGroup->name + " > Patterns");
	window->m_Position = CVector2D(20.0f, 120.0f);

	if (lightGroup->patternsData.size() > 0) {
		for (auto patternData : lightGroup->patternsData)
		{
			auto button = window->AddButton("[ Time: " + std::to_string(patternData->duration) + ", Pattern: " + patternData->pattern->name + " ]");
			button->m_OnClick = [lightGroup, patternData]() mutable {
				Menu::RemoveAllWindows();
				CreateMenu_SelectPatterns_Edit(patternData, [patternData, lightGroup](bool remove) {

					Menu::RemoveAllWindows();

					if (remove) {
						Vehicles::RemoveAllVehicles();
						lightGroup->RemovePattern(patternData);
						Vehicles::TryAddAllVehicles();

						CMessages::AddMessageJumpQ("removed", 500, 0, false);
					}
					else {}
					
					CreateMenu_EditLightGroup_EditPatterns(lightGroup);

				});
			};
		}

		window->AddDivider(5.0f);
	}

	auto addPattern = window->AddButton("Add pattern");
	addPattern->m_OnClick = [lightGroup]() {

		CreateMenu_SelectPatterns_Select(
			[lightGroup](PatternData* patternData) {
				lightGroup->AddPattern(patternData);

				Menu::RemoveAllWindows();
				CreateMenu_EditLightGroup_EditPatterns(lightGroup);
			},
			[lightGroup]() {
				Menu::RemoveAllWindows();
				CreateMenu_EditLightGroup_EditPatterns(lightGroup);
			}
			);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [lightGroup]() {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup(lightGroup);
	};
}

void Mod::CreateMenu_EditLightGroup_Points(LightGroup* lightGroup) {
	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + lightGroup->name + " Points");
	window->m_Position = CVector2D(20.0f, 120.0f);

	int i = 0;
	for (auto point : lightGroup->points)
	{
		auto buttonEditPoint = (Button*)window->AddItem((Item*)new Button());
		buttonEditPoint->m_Label = "[ Edit point " + std::to_string(i + 1) + " ]";
		buttonEditPoint->m_OnClick = [lightGroup, point]() mutable {
			Menu::RemoveAllWindows();
			CreateMenu_EditLightGroup_Point(lightGroup, point);
		};

		i++;
	}

	auto buttonAddPoint = window->AddButton("Add point");
	buttonAddPoint->m_OnClick = [lightGroup]() {

		lightGroup->AddPoint(CVector(0, 0, 2));

		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup_Points(lightGroup);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [lightGroup]() {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup(lightGroup);
	};
}

void Mod::CreateMenu_EditLightGroup_Point(LightGroup* lightGroup, Point* point) {
	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + lightGroup->name + " Points > " + "Point");
	window->m_Position = CVector2D(20.0f, 120.0f);

	auto positionButton = window->AddButton("Edit position");
	positionButton->m_OnClick = [point]() {
		PositionEditor::Toggle(&point->m_Position);
	};

	auto buttonDelete = window->AddButton("Delete point");
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [lightGroup, point]() {
		if (lightGroup->points.size() < 2) {
			return;
		}

		lightGroup->RemovePoint(point);

		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup(lightGroup);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [lightGroup]() {
		Menu::RemoveAllWindows();
		CreateMenu_EditLightGroup(lightGroup);
	};
}

void Mod::CreateMenu_EditColorTexture(ColorTexture* colorTexture) {
	CVehicle* veh = FindPlayerVehicle(0, false);

	auto window = Menu::AddWindow(m_MenuTitle, "ColorTextures > " + colorTexture->name);
	window->m_Position = CVector2D(20.0f, 120.0f);

	//CreateMenuTitle(window, "Vehicle ID " + std::to_string(veh->m_nModelIndex) + " > ColorTextures > " + colorTexture->name);

	auto buttonName = window->AddButton("Edit name (" + colorTexture->name + ")");
	buttonName->m_OnClick = [colorTexture]() mutable {
		Menu::SaveIndex();
		TextEditor::Open("Name", &colorTexture->name);
		TextEditor::m_OnConfirm = [colorTexture]() mutable {
			Menu::RemoveAllWindows();
			CreateMenu_EditColorTexture(colorTexture);
			Menu::RestoreIndex();
		};
	};

	auto buttonTexture = window->AddButton("Edit texture name (" + colorTexture->texture + ")");
	buttonTexture->m_OnClick = [colorTexture]() mutable {
		static std::string tmpStr;
		tmpStr = colorTexture->texture;

		Menu::SaveIndex();
		TextEditor::Open("Texture name", &tmpStr);
		TextEditor::m_OnConfirm = [colorTexture]() mutable {
			colorTexture->texture = tmpStr;

			Menu::RemoveAllWindows();
			CreateMenu_EditColorTexture(colorTexture);
			Menu::RestoreIndex();
		};
	};

	auto editPatterns = window->AddButton("Edit patterns (" + std::to_string(colorTexture->patternsData.size()) + " patterns)");
	editPatterns->m_OnClick = [colorTexture]() mutable {
		Menu::RemoveAllWindows();
		CreateMenu_EditColorTexture_EditPatterns(colorTexture);
	};

	auto checkBoxLerp = window->AddCheckBox("Lerp color", &colorTexture->lerpColor);

	auto checkBoxSiren = window->AddCheckBox("Turn on siren", &colorTexture->turnOnSiren);

	auto buttonKey = window->AddButtonKey("Key to toggle", colorTexture->keys);

	auto buttonDelete = window->AddButton("Delete color texture");
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [colorTexture]() {

		Vehicles::RemoveAllVehicles();
		ColorTextures::RemoveColorTexture(colorTexture);
		Vehicles::TryAddAllVehicles();

		Menu::RemoveAllWindows();
		CreateMenu_EditColorTextures();
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditColorTextures();
	};
}

void Mod::CreateMenu_EditColorTexture_EditPatterns(ColorTexture* colorTexture) {
	auto window = Menu::AddWindow(m_MenuTitle, "ColorTextures > " + colorTexture->name + " > Patterns");
	window->m_Position = CVector2D(20.0f, 120.0f);

	if (colorTexture->patternsData.size() > 0) {
		for (auto patternData : colorTexture->patternsData)
		{
			auto button = window->AddButton("[ Time: " + std::to_string(patternData->duration) + ", Pattern: " + patternData->pattern->name + " ]");
			button->m_OnClick = [colorTexture, patternData]() mutable {
				Menu::RemoveAllWindows();
				CreateMenu_SelectPatterns_Edit(patternData, [patternData, colorTexture](bool remove) {

					if (remove) {
						Vehicles::RemoveAllVehicles();
						colorTexture->RemovePattern(patternData);
						Vehicles::TryAddAllVehicles();
					}
					else {}

					Menu::RemoveAllWindows();
					CreateMenu_EditColorTexture_EditPatterns(colorTexture);

					});
			};
		}

		window->AddDivider(5.0f);
	}

	auto addPattern = window->AddButton("Add pattern");
	addPattern->m_OnClick = [colorTexture]() {
		CreateMenu_SelectPatterns_Select(
			[colorTexture](PatternData* patternData) {
				colorTexture->AddPattern(patternData);

				Menu::RemoveAllWindows();
				CreateMenu_EditColorTexture_EditPatterns(colorTexture);
			},
			[colorTexture]() {
				Menu::RemoveAllWindows();
				CreateMenu_EditColorTexture_EditPatterns(colorTexture);
			}
			);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [colorTexture]() {
		Menu::RemoveAllWindows();
		CreateMenu_EditColorTexture(colorTexture);
	};
}

/*
void Mod::CreateMenu_EditColorTexture_EditPattern(ColorTexture* colorTexture, ColorTexturePattern* colorTexturePattern) {
	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + colorTexture->name + " > Patterns > Pattern");
	window->m_Position = CVector2D(20.0f, 120.0f);

	auto time = window->AddNumberRange("Time", &colorTexturePattern->duration, 0, 999999);
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto buttonDelete = window->AddButton("Delete");
	buttonDelete->m_OnClick = [colorTexture, colorTexturePattern]() {

		Vehicles::RemoveAllVehicles();
		Vehicles::TryAddAllVehicles();

		LightGroups::RemovePatternReferences(colorTexturePattern->pattern);
		ColorTextures::RemovePatternReferences(colorTexturePattern->pattern);

		colorTexture->RemovePattern(colorTexturePattern->pattern);

		Menu::RemoveAllWindows();
		CreateMenu_EditColorTexture_EditPatterns(colorTexture);

	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [colorTexture]() {
		Menu::RemoveAllWindows();
		CreateMenu_EditColorTexture_EditPatterns(colorTexture);
	};
}

*/

void Mod::CreateMenu_SelectPatterns_Select(std::function<void(PatternData*)>onConfirm, std::function<void(void)> onCancel) {
	auto window = Menu::AddWindow(m_MenuTitle, "Select pattern");
	window->m_Position = CVector2D(430.0f, 120.0f);

	static int patternVal = 0;
	static int timeVal = 0;
	timeVal = 5000;

	auto options = window->AddOptions("Pattern", &patternVal);
	int i = 0;
	for (auto pattern : Patterns::m_Patterns) {
		options->AddOption(pattern->name, i);
		i++;
	}

	auto time = window->AddNumberRange("Time", &timeVal, 0, 999999);
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto add = window->AddButton("Add");
	add->m_OnClick = [onConfirm]() {
		auto patternData = new PatternData();
		patternData->pattern = Patterns::m_Patterns[patternVal];
		patternData->duration = timeVal;

		onConfirm(patternData);
	};

	auto cancel = window->AddButton("Cancel");
	cancel->m_OnClick = [onCancel]() {
		onCancel();
	};

	/*
	CreateMenu_NewPattern(std::function<void(Pattern*, int)>onConfirm, std::function<void(void)> onCancel);
	*/
}

void Mod::CreateMenu_SelectPatterns_Edit(PatternData* patternData, std::function<void(bool)> onClose) {
	auto window = Menu::AddWindow(m_MenuTitle, "LightGroups > " + std::string("Name") + "> Patterns > Pattern");
	window->m_Position = CVector2D(20.0f, 120.0f);

	auto time = window->AddNumberRange("Time", &patternData->duration, 0, 999999);
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto buttonDelete = window->AddButton("Delete");
	buttonDelete->m_OnClick = [onClose]() {
		onClose(true);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [onClose]() {
		onClose(false);
	};
}


void Mod::CreateMenu_EditPattern(Pattern* pattern) {
	auto window = Menu::AddWindow(m_MenuTitle, "Patterns > " + pattern->name);
	window->m_Position = CVector2D(20.0f, 120.0f);

	int i = 0;
	for (auto step : pattern->steps)
	{
		auto button = window->AddButton("Time: " + std::to_string(step->time));
		button->m_ColorIndicatorValue = &step->color;
		button->m_ColorIndicatorX = 5.0f;
		button->m_ColorIndicatorSize = CVector2D(20, 20);
		button->m_LabelMargin = CVector2D(35, 0);
		button->m_OnClick = [pattern, step]() mutable {
			Menu::RemoveAllWindows();
			CreateMenu_EditPatternStep(pattern, step);
		};
		i++;
	}

	window->AddDivider(5.0f);


	auto addColor = window->AddButton("Add color");
	addColor->m_OnClick = [pattern]() mutable {
		pattern->AddColor(CRGBA(255, 0, 0), 500);

		Menu::RemoveAllWindows();
		CreateMenu_EditPattern(pattern);
	};

	auto buttonName = window->AddButton("Edit name (" + pattern->name + ")");
	buttonName->m_OnClick = [pattern]() mutable {
		Menu::SaveIndex();
		TextEditor::Open("Name", &pattern->name);
		TextEditor::m_OnConfirm = [pattern]() mutable {
			Menu::RemoveAllWindows();
			CreateMenu_EditPattern(pattern);
			Menu::RestoreIndex();
		};
	};

	auto buttonDelete = window->AddButton("Delete");
	buttonDelete->m_OnClick = [pattern]() mutable {

		Vehicles::RemoveAllVehicles();
		LightGroups::RemovePatternReferences(pattern);
		ColorTextures::RemovePatternReferences(pattern);
		Patterns::RemovePattern(pattern);
		Vehicles::TryAddAllVehicles();

		Menu::RemoveAllWindows();
		CreateMenu_EditPatterns();
	};


	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = []() {
		Menu::RemoveAllWindows();
		CreateMenu_EditPatterns();
	};
}

void Mod::CreateMenu_EditPatternStep(Pattern* pattern, PatternStep* step) {
	auto window = Menu::AddWindow(m_MenuTitle, "Patterns > " + pattern->name + " > Step");
	window->m_Position = CVector2D(20.0f, 120.0f);

	auto colorButton = window->AddButton("Edit color");
	colorButton->m_ColorIndicatorValue = &step->color;
	colorButton->m_ColorIndicatorSize = CVector2D(40, 20);
	colorButton->m_ColorIndicatorX = colorButton->m_Size.x - colorButton->m_ColorIndicatorSize.x - 5;
	colorButton->m_OnClick = [pattern, step]() {
		Menu::SaveIndex();
		Menu::CreateColorPickerWindow(&step->color, [pattern, step]() mutable {
			Menu::RemoveAllWindows();
			CreateMenu_EditPatternStep(pattern, step);
			Menu::RestoreIndex();
			});
	};

	auto time = window->AddNumberRange("Time", &step->time, -9999999, 9999999);
	time->m_HoldToChange = true;

	auto buttonDelete = window->AddButton("Delete");
	buttonDelete->m_OnClick = [pattern, step]() mutable {
		Vehicles::RemoveAllVehicles();

		pattern->RemoveStep(step);

		Vehicles::TryAddAllVehicles();

		Menu::RemoveAllWindows();
		CreateMenu_EditPattern(pattern);
	};

	auto buttonBack = window->AddButton("Back");
	buttonBack->m_OnClick = [pattern]() mutable {
		Menu::RemoveAllWindows();
		CreateMenu_EditPattern(pattern);
	};
}

//


void Mod::ToggleMenu() {
	if (!Menu::m_Visible) {
		CVehicle* veh = FindPlayerVehicle(0, false);

		if (!veh)
		{
			Language::PrintText("need_vehicle", 1000);
			return;
		}
	}

	Menu::SetOpen(!Menu::m_Visible);
	m_MainMenuVisible = Menu::m_Visible;

	if (Menu::m_Visible) {
		CreateMenu_Main();
		SetPlayerControls(false);
	}
	else {
		Menu::RemoveAllWindows();
		SetPlayerControls(true);

		SaveToJSON();
	}
}

void Mod::Update() {
	Input::Update();

	for (auto pair : Vehicles::m_Vehicles)
	{
		Vehicle* hydra = pair.second;
		hydra->Update();
	}

	if (Input::GetKeyDown(88)) {
		ToggleMenu();
	}

	if (Input::GetKey(17)) {
		if (Input::GetKeyDown(82)) {
			CMessages::AddMessageJumpQ("Config reloaded", 1000, 0, false);

			if (m_MainMenuVisible) {
				ToggleMenu();
			}

			ResetConfig();
		}
	}

	if (Input::GetKey(17) && Input::GetKey(16)) {
		if (Input::GetKeyDown(82)) {
			m_ShowDebug = !m_ShowDebug;
			CMessages::AddMessageJumpQ(m_ShowDebug ? "Debug enabled" : "Debug disabled", 1000, 0, false);
		}
	}

	if (Menu::m_Visible) {
		if (Input::GetKeyDown(18)) {
			SetPlayerControls(true);
		}

		if (Input::GetKeyUp(18)) {
			SetPlayerControls(false);
		}
	}


	if (m_MainMenuVisible) {
		CVehicle* veh = FindPlayerVehicle(0, false);

		if (!veh)
		{
			ToggleMenu();
		}
	}

	Menu::Update();

}

void Mod::Draw() {
	for (auto pair : Vehicles::m_Vehicles)
	{
		Vehicle* vehicle = pair.second;
		vehicle->Draw();

		if (m_ShowDebug) {
			vehicle->DrawDebug();
		}
		
	}
	

	Menu::Draw();
}

void Mod::SetPlayerControls(bool enabled) {
	Command<Commands::SET_PLAYER_CONTROL>(0, enabled);
}

void Mod::SaveToJSON() {
	Patterns::SaveJSON();
	LightGroups::SaveJSON();
	ColorTextures::SaveJSON();
}

void Mod::LoadFromJSON() {
	Patterns::LoadJSON();
	LightGroups::LoadJSON();
	ColorTextures::LoadJSON();
}

void Mod::ResetConfig() {
	Vehicles::RemoveAllVehicles();

	LightGroups::RemoveAllLightGroups();
	ColorTextures::RemoveAllColorTextures();
	Patterns::RemovePatterns();

	LoadFromJSON();

	Vehicles::TryAddAllVehicles();
}

Mod::Mod() {
	Log::Open();
	Log::file << "Initialized" << std::endl;

	LoadFromJSON();

	if (Patterns::m_Patterns.size() == 0) {
		Patterns::CreateDefaultPattern("red blue");
	}

	
	//CreateTestMenu();

	SaveToJSON();

	//static std::string str1 = "Text";
	//TextEditor::Open("str1", &str1);

	//CreateTestSelectionMenu();



	Events::drawingEvent += []() {
		Draw();
	};

	Events::processScriptsEvent += []() {
		Update();
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