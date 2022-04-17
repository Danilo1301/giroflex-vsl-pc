#pragma once

#include "Item.h"

class Button : public Item {
private:
public:
	Button();

	std::function<void(void)> m_OnClick;

	void Update();
	void Draw(float x, float y);
	void OnButtonClicked();
};