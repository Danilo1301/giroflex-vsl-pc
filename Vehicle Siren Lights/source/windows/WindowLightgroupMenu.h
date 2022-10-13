#pragma once

#include "plugin.h"

#include "../LightGroup.h"
#include "../menu/Menu.h"

class WindowLightgroupMenu {
public:
	static std::vector<LightGroup*> m_LightGroups;
	static Window* m_Window;

	static void Create();
	static void Reopen();
	static void Check();
};