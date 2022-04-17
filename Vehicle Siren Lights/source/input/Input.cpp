#include "Input.h"
#include "vector"
#include "windows.h"

std::vector<int> keysPressed;
std::vector<int> keysDown;
std::vector<int> keysUp;

void Input::UpdateKeyState(int key) {
	if (GetKeyState(key) & 0x8000) {
		if (std::find(keysPressed.begin(), keysPressed.end(), key) == keysPressed.end()) {
			keysPressed.push_back(key);
			keysDown.push_back(key);
		}
	}
	else {
		if (std::find(keysPressed.begin(), keysPressed.end(), key) != keysPressed.end()) {
			keysPressed.erase(std::find(keysPressed.begin(), keysPressed.end(), key));
			keysUp.push_back(key);
		}
	}
}

bool Input::GetKey(int key) {
	UpdateKeyState(key);
	return std::find(keysPressed.begin(), keysPressed.end(), key) != keysPressed.end();
}

bool Input::GetKeyDown(int key) {
	UpdateKeyState(key);
	return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end();
}

bool Input::GetKeyUp(int key) {
	UpdateKeyState(key);
	return std::find(keysUp.begin(), keysUp.end(), key) != keysUp.end();
}

void Input::Update() {
	keysDown.clear();
	keysUp.clear();
}

bool Input::CheckKeyStroke(std::vector<int> keys) {
	for (auto k : keys)
	{
		UpdateKeyState(k);
	}

	int lastKey = 0;
	int total = 0;
	for (int i = 0; i < (int)keys.size(); i++)
	{
		if (keys[i] != -1) {
			lastKey = keys[i];
			total++;
		}
	}

	int pressed = 0;
	for (int i = 0; i < total; i++)
	{
		if (keys[i] != lastKey) {
			if (Input::GetKey(keys[i])) pressed++;
		}
		else {
			if (Input::GetKeyDown(keys[i])) pressed++;
		}
	}

	return (total > 0 && total == pressed);
}