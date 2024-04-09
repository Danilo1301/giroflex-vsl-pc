#pragma once

#include "LightGroup.h"

class LightGroups {
public:
	static std::map<int, std::vector<LightGroup*>> m_LightGroups;

	static LightGroup* CreateLightGroup(int modelId, std::string fileName);

	static void RemoveLightGroup(LightGroup* lightGroup);
	static std::vector<LightGroup*> GetLightGroups(int modelId);
	static bool HasLightGroups(int modelId);

	static void RemovePatternReferences(Pattern* removePattern);

	static void RemoveAllLightGroups();

	static LightGroup* FindLightGroupByFileName(int model, std::string fileName);
	static void FindUniqueName(LightGroup* lightGroup, std::string prefix);
};