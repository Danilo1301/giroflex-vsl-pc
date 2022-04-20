#pragma once

#include "pch.h"
#include "Pattern.h"

class ColorTexture {
public:
	std::vector<PatternData*> patternsData;
	std::string name;
	std::string texture;

	int modelId = -1;
	bool lerpColor = false;
	bool turnOnSiren = false;

	int keys[3] = { 17, 76, -1 };

	ColorTexture(int modelId, std::string name, std::string texture) {
		this->modelId = modelId;
		this->name = name;
		this->texture = texture;
	}

	void Destroy() {
		while (patternsData.size() > 0) {
			RemovePattern(patternsData[0]);
		}
	}

	void AddPattern(Pattern* pattern, int duration) {
		PatternData* patternData = new PatternData();
		patternData->pattern = pattern;
		patternData->duration = duration;
		AddPattern(patternData);
	}

	void AddPattern(PatternData* patternData) {
		patternsData.push_back(patternData);
	}

	void RemovePattern(PatternData* patternData) {
		std::vector<PatternData*>::iterator it;
		for (it = patternsData.begin(); it != patternsData.end(); it++) {
			auto value = (*it);
			if (value == patternData) {
				Log::file << "Pattern " << patternData->pattern->name << " removed from ColorTexture " << name << std::endl;
				patternsData.erase(it);
				delete value;
				return;
			}
		}
	}

	Json::Value ToJSON() {
		Json::Value json;

		json["modelId"] = modelId;
		json["lerpColor"] = lerpColor;
		json["turnOnSiren"] = turnOnSiren;
		json["name"] = name;
		json["texture"] = texture;

		json["patterns"] = Json::arrayValue;
		for (auto patternData : patternsData)
		{
			json["patterns"].append(PatternDataToJSON(patternData));
		}

		json["keys"] = Json::arrayValue;
		for (size_t i = 0; i < 3; i++) json["keys"][i] = keys[i];

		return json;
	}

	void FromJson(Json::Value json) {
		modelId = json["modelId"].asInt();
		lerpColor = json["lerpColor"].asBool();
		turnOnSiren = json["turnOnSiren"].asBool();
		name = json["name"].asString();
		texture = json["texture"].asString();

		patternsData.clear();
		for (int i = 0; i < (int)json["patterns"].size(); i++)
		{
			patternsData.push_back(PatternDataFromJSON(json["patterns"][i]));
		}

		for (size_t i = 0; i < 3; i++) keys[i] = json["keys"][i].asInt();
	}
};

class ColorTextures {
public:
	static std::map<int, std::vector<ColorTexture*>> m_ColorTextures;

	static ColorTexture* CreateColorTexture(int modelId, std::string name, std::string texture) {
		ColorTexture* colorTexture = new ColorTexture(modelId, name, texture);
		m_ColorTextures[modelId].push_back(colorTexture);
		return colorTexture;
	}

	static ColorTexture* CreateDefaultColorTexture(int modelId, std::string name, std::string texture) {
		auto colorTexture = CreateColorTexture(modelId, name, texture);

		if (Patterns::m_Patterns.size() > 0) {
			colorTexture->AddPattern(Patterns::m_Patterns[0], 7000);
		}

		return colorTexture;
	}

	static void RemoveColorTexture(ColorTexture* colorTexture) {
		for (auto pair : m_ColorTextures) {
			auto id = pair.first;

			auto it = std::find(m_ColorTextures[id].begin(), m_ColorTextures[id].end(), colorTexture);
			if (it == m_ColorTextures[id].end()) continue;
			m_ColorTextures[id].erase(it);
			colorTexture->Destroy();
			delete colorTexture;
		}
	}

	static std::vector<ColorTexture*> GetColorTextures(int modelId) {
		return m_ColorTextures[modelId];
	}

	static bool HasColorTextures(int modelId) {
		return m_ColorTextures.find(modelId) != m_ColorTextures.end();
	}

	static void RemovePatternReferences(Pattern* removePattern) {
		Log::file << "Trying to remove pattern " << removePattern->name << " references from ColorTextures" << std::endl;

		for (auto pair : m_ColorTextures) {
			auto colorTextures = pair.second;

			for (auto colorTexture : colorTextures) {
				std::vector<PatternData*> toRemove;

				for (auto patternData : colorTexture->patternsData) {
					if (patternData->pattern == removePattern) {
						toRemove.push_back(patternData);
					}
				}

				for (auto item : toRemove) {
					colorTexture->RemovePattern(item);
				}
			}
		}
	}

	static void SaveJSON() {
		Json::Value colorTexturesJson = Json::arrayValue;

		for (auto pair : m_ColorTextures) {
			auto id = pair.first;
			auto colorTextures = pair.second;

			for (auto colorTexture : colorTextures) {
				auto json = colorTexture->ToJSON();

				colorTexturesJson.append(json);
			}
		}

		Json::StyledWriter writer;
		std::string strJson = writer.write(colorTexturesJson);

		std::string path = PLUGIN_PATH("\\data\\colortextures.json");
		std::ofstream file(path);
		file << strJson;
		file.close();
	}

	static void LoadJSON() {
		std::string path = PLUGIN_PATH("\\data\\colortextures.json");
		std::ifstream file(path);

		Json::Value json;
		Json::Reader reader;

		if (!reader.parse(file, json, true)) {
			MessageBox(HWND_DESKTOP, "Error loading lightgroups.json", "Vehicle Siren Lights", MB_ICONERROR);
			return;
		}

		for (int i = 0; i < (int)json.size(); i++)
		{
			auto colorTexturesJson = json[i];

			int modelId = colorTexturesJson["modelId"].asInt();

			auto colorTexture = CreateColorTexture(modelId, "", "");
			colorTexture->FromJson(colorTexturesJson);
		}
	}

	static void RemoveAllColorTextures() {
		std::vector<ColorTexture*> toremove;
		for (auto a : m_ColorTextures) {
			for (auto b : a.second) toremove.push_back(b);
		}
		for (auto a : toremove) RemoveColorTexture(a);
	}
};