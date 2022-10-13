#pragma once

#include "Item.h"

#include "../../Keybind.h"

class ButtonKey : public Item {
public:
	ButtonKey(Keybind* keybind);

	Keybind* m_Keybind;

	void Update();
	void Draw(float x, float y);
	void DrawTextDisplay(float x, float y, float width, float height);
};