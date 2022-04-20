#pragma once

#include "Item.h"

class CheckBox : public Item {
private:
public:
	bool* m_Value;

	CheckBox(bool* value);

	void Update();
	void Draw(float x, float y);
};