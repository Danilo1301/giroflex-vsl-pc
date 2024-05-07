#include "LightGroups.h"
#include "TestHelper.h"

#include "log/Log.h"

std::map<int, std::vector<LightGroup*>> LightGroups::m_LightGroups;

LightGroup* LightGroups::CreateLightGroup(int modelId, std::string fileName)
{
	Log::file << "[LightGroups] Create LightGroup '" << fileName << "' for model ID " << modelId << std::endl;

	LightGroup* lightGroup = new LightGroup(modelId);
	
	if (fileName.empty()) FindUniqueName(lightGroup, "lightGroup");
	else lightGroup->fileName = fileName;

	m_LightGroups[modelId].push_back(lightGroup);

	//fix modelDirectoryEntry when creating new lightGroup
	auto otherLightGroups = LightGroups::GetLightGroups(modelId);
	if (otherLightGroups.size() > 0)
	{
		lightGroup->modelDirectoryEntry = otherLightGroups[0]->modelDirectoryEntry;
	}

	return lightGroup;
}

void LightGroups::RemoveLightGroup(LightGroup* lightGroup) {
	Log::file << "[LightGroups] RemoveLightGroup " << lightGroup->fileName << std::endl;

	for (auto pair : m_LightGroups) {
		auto id = pair.first;

		auto it = std::find(m_LightGroups[id].begin(), m_LightGroups[id].end(), lightGroup);
		if (it == m_LightGroups[id].end()) continue;
		m_LightGroups[id].erase(it);
		lightGroup->Destroy();
		delete lightGroup;
	}
}

std::vector<LightGroup*> LightGroups::GetLightGroups(int modelId) {
	return m_LightGroups[modelId];
}

bool LightGroups::HasLightGroups(int modelId) {
	auto it = m_LightGroups.find(modelId);

	if (it != m_LightGroups.end()) {
		if ((*it).second.size() == 0) return false;
		return true;
	}

	return false;
}

void LightGroups::RemovePatternReferences(Pattern* pattern) {
	Log::file << "[LightGroups] Trying to remove pattern '" << pattern->name << "' references" << std::endl;

	for (auto pair : m_LightGroups) {
		auto lightGroups = pair.second;

		for (auto lightGroup : lightGroups) {
			std::vector<PatternCycleStep*> toRemove;

			for (auto patternCycleStep : lightGroup->patternCycleSteps) {
				if (patternCycleStep->pattern == pattern) {
					toRemove.push_back(patternCycleStep);
				}
			}

			for (auto item : toRemove) {
				lightGroup->RemovePatternCycleStep(item);
			}
		}
	}
}

void LightGroups::RemoveAllLightGroups() {
	Log::file << "[LightGroups] RemoveAllLightGroups" << std::endl;

	std::vector<LightGroup*> toremove;
	for (auto pair : m_LightGroups) {
		while (m_LightGroups[pair.first].size() > 0) {
			RemoveLightGroup(m_LightGroups[pair.first][0]);
		}
	}
}

LightGroup* LightGroups::FindLightGroupByFileName(int model, std::string fileName)
{
	TestHelper::AddLine("[FindLightGroupByFileName] " + fileName);

	auto& lightGroups = m_LightGroups[model];

	for (auto lightGroup : lightGroups)
	{
		TestHelper::AddLine("[FindLightGroupByFileName] compare with " + lightGroup->fileName);
		if (fileName.compare(lightGroup->fileName) == 0) return lightGroup;
	}

	TestHelper::AddLine("[FindLightGroupByFileName] found none with this fileName");

	return NULL;
}

void LightGroups::FindUniqueName(LightGroup* lightGroup, std::string prefix)
{
	TestHelper::AddLine("Find name.. Was: '" + lightGroup->fileName + "'");

	int i = 0;
	while (lightGroup->fileName.empty())
	{
		std::string newName = prefix + std::to_string(i);

		TestHelper::AddLine("Testing '" + newName + "'");

		auto sameName = FindLightGroupByFileName(lightGroup->modelId, newName);
		if (sameName)
		{
			TestHelper::AddLine("Already exists");
			i++;
		}
		else {
			TestHelper::AddLine("Available");
			lightGroup->fileName = newName;
		}
	}

	TestHelper::AddLine("Set to '" + lightGroup->fileName + "'");
}