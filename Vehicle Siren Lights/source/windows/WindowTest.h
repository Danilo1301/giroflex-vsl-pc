#pragma once

#include "Mod.h"

int testInt = 0;

class WindowTest {
public:
	static void Create() {
		auto window = Menu::AddWindow("title", "test");

		auto testint = window->AddNumberRange("testint", &testInt, 0, 5000);
		testint->m_HoldToChange = true;

		for (int i = 0; i < 30; i++)
		{
			auto button = window->AddButton("Select " + std::to_string(i));
			button->m_OnClick = [i]() mutable {

			};
		}

		//Menu::SetOpen(true);
	}
};