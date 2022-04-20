#pragma once

#include "Item.h"

class ButtonKey : public Item {
public:
	ButtonKey(int* value);

	int* m_Keys;

	void Update();
	void Draw(float x, float y);
	void DrawTextDisplay(float x, float y, float width, float height);

	static ButtonKey* m_EditingButtonKey;
};