#pragma once

#include "Menu.h"

class PositionEditor {
public:
	static bool m_Visible;
	static CVector* m_Value;

	static void Update();
	static void Draw();
	static void Toggle(CVector* value);
};