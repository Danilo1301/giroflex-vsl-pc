#pragma once

#include "Mod.h"

#include "../menu/Menu.h"

#include "../menu/TextEditor.h"

std::string testString = "String";
int testInt = 1;
unsigned char testUChar = 25;
float testFloat = 0.5f;

class WindowTest {
public:
	static void Create() {
		
		auto window = Menu::AddWindow("title", "test");

		auto buttonString = window->AddButton("Edit test string");
		buttonString->AddTextStr(&testString, CVector2D(10, 0));
		buttonString->m_OnClick = []() {
			TextEditor::Open("Edit string", true, &testString);
		};

		//int
		auto rangeTestInt = window->AddNumberRange("Test int", &testInt, -100, 100);
		rangeTestInt->m_HoldToChange = true;

		auto buttonEditInt = window->AddButton("Edit int");
		buttonEditInt->m_OnClick = []() {
			TextEditor::Open("Edit int", true, &testInt);
		};

		//uchar
		auto rangeTestUChar = window->AddNumberRange("Test uchar", &testUChar, (unsigned char)0, (unsigned char)255);
		rangeTestUChar->m_HoldToChange = true;

		auto buttonEditUChar = window->AddButton("Edit uchar");
		buttonEditUChar->m_OnClick = []() {
			TextEditor::Open("Edit uchar", true, &testUChar);
		};

		//float
		auto rangeTestFloat = window->AddNumberRange("Test float", &testFloat, -10.0f, 10.0f);
		rangeTestFloat->m_HoldToChange = true;

		auto buttonEditFloat = window->AddButton("Edit float");
		buttonEditFloat->m_OnClick = []() {
			TextEditor::Open("Edit float", true, &testFloat);
		};

		for (int i = 0; i < 2; i++)
		{
			auto button = window->AddButton("Select " + std::to_string(i));
			button->m_OnClick = [i]() mutable {

			};
		}
	}
};