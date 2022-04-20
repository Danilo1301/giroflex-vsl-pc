#pragma once

#include "Item.h"

struct Option {
	std::string text;
	int value;
};

class Options : public Item {
public:
	std::vector<Option> m_Options;

	std::function<void(void)> m_OnChange;

	int* m_Value;

	Options(int* value);

	void Update();
	void Draw(float x, float y);
	void DrawNumberDisplay(float x, float y, float width, float height);

	void AddOption(std::string text, int value);
	int GetOptionIndexByValue(int value);
	void ChangeOption(int by);
};