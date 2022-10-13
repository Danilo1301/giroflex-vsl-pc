#include "LightGroups.h"

#include "log/Log.h"

std::map<int, std::vector<LightGroup*>> LightGroups::m_LightGroups;

LightGroup* LightGroups::CreateLightGroup(int modelId)
{
	Log::file << "[LightGroups] CreateLightGroup for model ID " << modelId << std::endl;

	LightGroup* lightGroup = new LightGroup(modelId);
	
	FindUniqueName(lightGroup);
	//Log::file << "[LightGroups] Default fileName set to: " << lightGroup->fileName << std::endl;

	m_LightGroups[modelId].push_back(lightGroup);

	return lightGroup;
}


LightGroup* LightGroups::CreateLightbarLightGroup(int modelId) {

	Log::file << "[LightGroups] CreateLightbarLightGroup for model ID " << modelId << std::endl;

	LightGroup* lightGroup = new LightGroup(modelId);
	lightGroup->lightbarSettings.isLightbar = true;
	lightGroup->UpdateLightbarPoints(11, 0, 0.1f);

	FindUniqueName(lightGroup);
	//Log::file << "[LightGroups] Default fileName set to: " << lightGroup->fileName << std::endl;

	m_LightGroups[modelId].push_back(lightGroup);

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

void LightGroups::FindUniqueName(LightGroup* lightGroup)
{
	Log::file << "[LightGroups] FindUniqueName for lightGroup..." << std::endl;

	int i = 0;
	while (lightGroup->fileName.empty())
	{
		std::string newName = "lightGroup" + std::to_string(i);

		auto& lightGroups = m_LightGroups[lightGroup->modelId];

		if (lightGroups.size() == 0)
		{
			lightGroup->fileName = newName;
		}
		else {
			for (auto l : lightGroups)
			{
				if (l->fileName.compare(newName) != 0)
				{
					lightGroup->fileName = newName;
					break;
				}
			}
		}
		i++;
	}
	Log::file << "[LightGroups] Found: " << lightGroup->fileName << std::endl;
}