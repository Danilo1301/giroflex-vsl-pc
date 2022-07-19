#include "Keybinds.h"

Keybind Keybinds::openMenu = { 76, false, false, true };
Keybind Keybinds::toggleLights = { 74, false, false, true };
Keybind Keybinds::editorUpDown = { -1, false, false, true };
Keybind Keybinds::editorSlower = { -1, false, true, false };

Json::Value Keybinds::ToJSON()
{
	Json::Value value = Json::objectValue;

	value["openMenu"] = openMenu.ToJSON();
	value["toggleLights"] = toggleLights.ToJSON();
	value["editorUpDown"] = editorUpDown.ToJSON();
	value["editorSlower"] = editorSlower.ToJSON();

	return value;
}

void Keybinds::FromJSON(Json::Value value)
{
	if (value.empty()) return;

	openMenu.FromJSON(value["openMenu"]);
	toggleLights.FromJSON(value["toggleLights"]);
	editorUpDown.FromJSON(value["editorUpDown"]);
	editorSlower.FromJSON(value["editorSlower"]);
}