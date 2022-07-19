#pragma once

#include "pch.h"

#include "Keybind.h"

class Keybinds {
public:
	static Keybind openMenu;
	static Keybind toggleLights;
	static Keybind editorUpDown;
	static Keybind editorSlower;

	static Json::Value ToJSON();
	static void FromJSON(Json::Value value);
};