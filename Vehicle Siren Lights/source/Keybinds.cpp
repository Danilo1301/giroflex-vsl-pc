#include "Keybinds.h"

Keybind Keybinds::openMenu("L", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::toggleDebug("D", KEYBIND_FLAGS::CTRL | KEYBIND_FLAGS::SHIFT);
Keybind Keybinds::toggleLights("J", KEYBIND_FLAGS::CTRL);
Keybind Keybinds::editorSlower("", KEYBIND_FLAGS::SHIFT);
Keybind Keybinds::reloadConfig("R", KEYBIND_FLAGS::CTRL);

Json::Value Keybinds::ToJSON()
{
	Json::Value value = Json::objectValue;

	value["openMenu"] = openMenu.ToJSON();
	value["toggleDebug"] = toggleDebug.ToJSON();
	value["toggleLights"] = toggleLights.ToJSON();
	value["editorSlower"] = editorSlower.ToJSON();
	value["reloadConfig"] = reloadConfig.ToJSON();

	return value;
}

void Keybinds::FromJSON(Json::Value value)
{
	if (value.empty()) return;

	if(!value["openMenu"].isNull()) openMenu.FromJSON(value["openMenu"]);
	if(!value["toggleDebug"].isNull()) toggleDebug.FromJSON(value["toggleDebug"]);
	if(!value["toggleLights"].isNull()) toggleLights.FromJSON(value["toggleLights"]);
	if(!value["editorSlower"].isNull()) editorSlower.FromJSON(value["editorSlower"]);
	if(!value["reloadConfig"].isNull()) reloadConfig.FromJSON(value["reloadConfig"]);
}