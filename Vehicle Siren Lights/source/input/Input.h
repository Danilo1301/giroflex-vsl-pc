#pragma once

#include "vector"
#include "windows.h"

class Input {
private:
	static void UpdateKeyState(int key);
public:
	static bool GetKey(int key);
	static bool GetKeyDown(int key);
	static bool GetKeyUp(int key);

	static void Update();

	static bool CheckKeyStroke(std::vector<int> keys);
};