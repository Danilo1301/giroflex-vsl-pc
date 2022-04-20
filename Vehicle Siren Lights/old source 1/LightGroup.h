#pragma once

#include "pch.h"
#include "Point.h"
#include "Pattern.h"

class LightGroup {
public:
	std::vector<Point*> points;
	std::vector<PatternData*> patternsData;
	std::string name = "Light group";

	CVector position = CVector(0, 0, 0);

	int modelId;
	int amount = 10;
	int offsetBy = 0;
	int offsetId = 0;
	float size = 0.2f;
	bool lerpColor = false;
	bool turnOnSiren = false;
	bool useFlash = false;

	int keys[3] = { 17, 76, -1 };

	LightGroup(int modelId) {
		this->modelId = modelId;
	}
	
	Point* AddPoint(CVector position) {
		Point* point = new Point();
		point->m_Position = position;
		points.push_back(point);
		return point;
	}

	void RemovePoint(Point* point) {
		auto it = std::find(points.begin(), points.end(), point);
		if (it == points.end()) return;
		points.erase(it);
		delete (*it);
	}

	void AddPattern(Pattern* pattern, int duration) {
		PatternData* patternData = new PatternData();
		patternData->pattern = pattern;
		patternData->duration = duration;
		AddPattern(patternData);
	}

	void AddPattern(PatternData* patternData) {
		patternsData.push_back(patternData);

		Log::file << "Pattern added to lightgroup" << std::endl;
	}

	void RemovePattern(PatternData* patternData) {
		auto it = std::find(patternsData.begin(), patternsData.end(), patternData);
		if (it == patternsData.end()) return;
		patternsData.erase(it);
		delete patternData;

		Log::file << "Pattern removed to lightgroup" << std::endl;
	}

	void Destroy() {
		while (patternsData.size() > 0) {
			RemovePattern(patternsData[0]);
		}

		while (points.size() > 0) {
			RemovePoint(points[0]);
		}
	}

	Json::Value ToJSON() {
		Json::Value json;

		json["modelId"] = modelId;
		json["name"] = name;
		json["amount"] = amount;
		json["offsetBy"] = offsetBy;
		json["offsetId"] = offsetId;
		json["size"] = size;
		json["lerpColor"] = lerpColor;
		json["turnOnSiren"] = turnOnSiren;
		json["useFlash"] = useFlash;
		json["position"] = CVectorToJSON(position);

		json["patterns"] = Json::arrayValue;
		for (auto patternData : patternsData)
		{
			json["patterns"].append(PatternDataToJSON(patternData));
		}
		
		json["points"] = Json::arrayValue;
		for (auto point : points)
		{
			Json::Value pointJson;
			pointJson["position"] = CVectorToJSON(point->m_Position);

			json["points"].append(pointJson);
		}

		json["keys"] = Json::arrayValue;
		for (size_t i = 0; i < 3; i++) json["keys"][i] = keys[i];

		return json;
	}

	void FromJson(Json::Value json) {
		modelId = json["modelId"].asInt();
		amount = json["amount"].asInt();
		name = json["name"].asString();
		offsetBy = json["offsetBy"].asInt();
		offsetId = json["offsetId"].asInt();
		size = json["size"].asFloat();
		lerpColor = json["lerpColor"].asBool();
		turnOnSiren = json["turnOnSiren"].asBool();
		useFlash = json["useFlash"].asBool();
		position = CVectorFromJSON(json["position"]);

		patternsData.clear();
		for (int i = 0; i < (int)json["patterns"].size(); i++)
		{
			patternsData.push_back(PatternDataFromJSON(json["patterns"][i]));
		}

		points.clear();
		for (int i = 0; i < (int)json["points"].size(); i++)
		{
			auto pointJson = json["points"][i];
			AddPoint(CVectorFromJSON(pointJson["position"]));
		}

		for (size_t i = 0; i < 3; i++) keys[i] = json["keys"][i].asInt();
	}

};

class LightGroups {
public:
	static std::map<int, std::vector<LightGroup*>> m_LightGroups;

	static LightGroup* CreateLightGroup(int modelId) {
		LightGroup* lightGroup = new LightGroup(modelId);
		m_LightGroups[modelId].push_back(lightGroup);
		return lightGroup;
	}

	static void RemoveLightGroup(LightGroup* lightGroup) {
		for (auto pair : m_LightGroups) {
			auto id = pair.first;

			auto it = std::find(m_LightGroups[id].begin(), m_LightGroups[id].end(), lightGroup);
			if (it == m_LightGroups[id].end()) continue;
			m_LightGroups[id].erase(it);
			lightGroup->Destroy();
			delete lightGroup;
		}
	}

	static LightGroup* CreateDefaultLightGroup(int modelId) {
		auto lightGroup = CreateLightGroup(modelId);
		lightGroup->AddPoint(CVector(0, 0, 0));
		lightGroup->position = CVector(0, 0, 2);

		if (Patterns::m_Patterns.size() > 0) {
			lightGroup->AddPattern(Patterns::m_Patterns[0], 7000);
		}

		return lightGroup;
	}

	static std::vector<LightGroup*> GetLightGroups(int modelId) {
		return m_LightGroups[modelId];
	}

	static bool HasLightGroups(int modelId) {
		return m_LightGroups.find(modelId) != m_LightGroups.end();
	}

	static void RemovePatternReferences(Pattern* removePattern) {
		Log::file << "Trying to remove pattern " << removePattern->name << " references from ColorTextures" << std::endl;

		for (auto pair : m_LightGroups) {
			auto lightGroups = pair.second;

			for (auto lightGroup : lightGroups) {
				std::vector<PatternData*> toRemove;

				for (auto patternData : lightGroup->patternsData) {
					if (patternData->pattern == removePattern) {
						toRemove.push_back(patternData);
					}
				}

				for (auto item : toRemove) {
					lightGroup->RemovePattern(item);
				}
			}
		}
	}

	static void SaveJSON() {
		Json::Value lightGroupsJson = Json::arrayValue;

		for (auto pair : m_LightGroups) {
			auto id = pair.first;
			auto lightGroups = pair.second;

			for (auto lightGroup : lightGroups) {
				auto json = lightGroup->ToJSON();

				lightGroupsJson.append(json);
			}
		}

		Json::StyledWriter writer;
		std::string strJson = writer.write(lightGroupsJson);

		std::string path = PLUGIN_PATH("\\data\\lightgroups.json");
		std::ofstream file(path);
		file << strJson;
		file.close();
	}

	static void LoadJSON() {
		std::string path = PLUGIN_PATH("\\data\\lightgroups.json");
		std::ifstream file(path);

		Json::Value json;
		Json::Reader reader;

		if (!reader.parse(file, json, true)) {
			MessageBox(HWND_DESKTOP, "Error loading lightgroups.json", "Vehicle Siren Lights", MB_ICONERROR);
			return;
		}

		for (int i = 0; i < (int)json.size(); i++)
		{
			auto lightGroupJson = json[i];

			int modelId = lightGroupJson["modelId"].asInt();

			auto lightGroup = CreateLightGroup(modelId);
			lightGroup->FromJson(lightGroupJson);
		}
	}

	static void RemoveAllLightGroups() {
		std::vector<LightGroup*> toremove;
		for (auto a : m_LightGroups) {
			for (auto b : a.second) toremove.push_back(b);
		}
		for (auto a : toremove) RemoveLightGroup(a);
	}
};