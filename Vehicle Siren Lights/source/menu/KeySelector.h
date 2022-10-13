#pragma once

#include "plugin.h"
#include "../Keybind.h"

class KeySelector {
public:
	static Keybind m_KeybindResult;
	static Keybind* m_KeybindTarget;
	static bool m_Visible;
	static std::function<void(void)> m_OnConfirm;

	static void Toggle(Keybind* keybind);
	static void Update();
	static void Draw();
};