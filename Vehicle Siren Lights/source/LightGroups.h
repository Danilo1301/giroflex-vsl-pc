#pragma once

#include "LightGroup.h"

class LightGroups {
public:
	static std::map<int, std::vector<LightGroup*>> m_LightGroups;

	static LightGroup* CreateLightGroup(int modelId);
	static void RemoveLightGroup(LightGroup* lightGroup);
	static std::vector<LightGroup*> GetLightGroups(int modelId);
	static bool HasLightGroups(int modelId);
};