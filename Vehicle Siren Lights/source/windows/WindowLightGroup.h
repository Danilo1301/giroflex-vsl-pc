#pragma once

#include "../LightGroups.h"

class WindowLightGroup {
public:
	static LightGroup* m_LightGroup;

	static void CreateLightGroups();
	static void CreateSetupLightbar_Pre();
	static void CreateSetupLightbar();
	static void CreateEditLightGroup();
};
