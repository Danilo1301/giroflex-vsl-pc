#include "Keybinds.h"

Keybind Keybinds::openMenu("L", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::toggleDebug("D", KEYBIND_FLAGS::CTRL | KEYBIND_FLAGS::SHIFT);
Keybind Keybinds::editorUpDown("", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::editorSlower("", KEYBIND_FLAGS::ALT);
Keybind Keybinds::reloadConfig("R", KEYBIND_FLAGS::CTRL);

Json::Value Keybinds::ToJSON()
{
	Json::Value value = Json::objectValue;

	value["openMenu"] = openMenu.ToJSON();
	value["toggleDebug"] = toggleDebug.ToJSON();
	value["editorUpDown"] = editorUpDown.ToJSON();
	value["editorSlower"] = editorSlower.ToJSON();
	value["reloadConfig"] = reloadConfig.ToJSON();

	return value;
}

void Keybinds::FromJSON(Json::Value value)
{
	if (value.empty()) return;

	/*
	* BUG
	* 
	if(!value["openMenu"].isNull()) openMenu.FromJSON(value["openMenu"]);
	if(!value["toggleDebug"].isNull()) openMenu.FromJSON(value["toggleDebug"]);
	if(!value["editorUpDown"].isNull()) openMenu.FromJSON(value["editorUpDown"]);
	if(!value["editorSlower"].isNull()) openMenu.FromJSON(value["editorSlower"]);
	if(!value["reloadConfig"].isNull()) openMenu.FromJSON(value["reloadConfig"]);
	*/
}