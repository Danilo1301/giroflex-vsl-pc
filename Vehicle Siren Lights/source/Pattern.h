#pragma once

#include "pch.h"

struct PatternStep {
	CRGBA color;
	int time;
};

class Pattern {
public:
	std::string name;
	std::vector<PatternStep*> steps;
	bool lerpColor = false;

	Pattern(std::string name) {
		this->name = name;
	}

	void AddColor(CRGBA color, int time) {
		PatternStep* step = new PatternStep();
		step->color = color;
		step->time = time;
		steps.push_back(step);
	}

	void RemoveStep(PatternStep* step) {
		steps.erase(std::find(steps.begin(), steps.end(), step));
		delete step;
	}

	void RemoveSteps() {
		while (steps.size() > 0)
		{
			RemoveStep(steps[0]);
		}
	}

	Json::Value ToJSON() {
		Json::Value json;

		json["name"] = name;
		json["lerpColor"] = lerpColor;

		json["steps"] = Json::arrayValue;
		for (auto step : steps)
		{
			Json::Value stepJson;
			stepJson["time"] = step->time;
			stepJson["color"] = ColorToJSON(step->color);

			json["steps"].append(stepJson);
		}

		return json;
	}

	void FromJson(Json::Value json) {
		name = json["name"].asString();
		lerpColor = json["lerpColor"].asBool();

		steps.clear();
		for (int i = 0; i < (int)json["steps"].size(); i++)
		{
			auto stepJson = json["steps"][i];
			auto time = stepJson["time"].asInt();
			auto color = ColorFromJSON(stepJson["color"]);

			AddColor(color, time);
		}
	}
};

struct PatternData {
	Pattern* pattern;
	int duration;
};

class Patterns {
public:
	static std::vector<Pattern*> m_Patterns;

	static Pattern* CreateDefaultPattern(std::string name) {
		Pattern* pattern = CreatePattern(name);

		pattern->AddColor(CRGBA(255, 0, 0), 500);
		pattern->AddColor(CRGBA(0, 0, 0), 500);
		pattern->AddColor(CRGBA(0, 0, 255), 500);
		pattern->AddColor(CRGBA(0, 0, 0), 500);;
		return pattern;
	}

	static Pattern* CreatePattern(std::string name) {
		Log::file << "CreatePattern " << name << " - total " << std::to_string(m_Patterns.size() + 1) << std::endl;

		Pattern* pattern = new Pattern(name);
		m_Patterns.push_back(pattern);
		return pattern;
	}

	static void RemovePattern(Pattern* pattern) {
		Log::file << "RemovePattern " << pattern->name << " - total " << std::to_string(m_Patterns.size() - 1) << std::endl;

		m_Patterns.erase(std::find(m_Patterns.begin(), m_Patterns.end(), pattern));
		delete pattern;
	}

	static void RemovePatterns() {
		Log::file << "RemovePatterns" << std::endl;

		while (m_Patterns.size() > 0)
		{
			RemovePattern(m_Patterns[0]);
		}
	}

	static void SaveJSON() {
		Json::Value patternsJson = Json::arrayValue;

		for (auto pattern : m_Patterns) {
			auto json = pattern->ToJSON();

			patternsJson.append(json);
		}

		Json::StyledWriter writer;
		std::string strJson = writer.write(patternsJson);

		std::string path = PLUGIN_PATH("\\data\\patterns.json");
		std::ofstream file(path);
		file << strJson;
		file.close();
	}

	static void LoadJSON() {
		std::string path = PLUGIN_PATH("\\data\\patterns.json");
		std::ifstream file(path);

		Json::Value json;
		Json::Reader reader;

		if (!reader.parse(file, json, true)) {
			MessageBox(HWND_DESKTOP, "Error loading patterns.json", "Vehicle Siren Lights", MB_ICONERROR);
			return;
		}

		for (int i = 0; i < (int)json.size(); i++)
		{
			auto patternJson = json[i];

			auto pattern = CreatePattern("");
			pattern->FromJson(patternJson);
		}
	}
};


static Json::Value PatternDataToJSON(PatternData* patternData) {
	Json::Value json;
	json["duration"] = patternData->duration;

	auto it = std::find(Patterns::m_Patterns.begin(), Patterns::m_Patterns.end(), patternData->pattern);
	auto index = it - Patterns::m_Patterns.begin();
	json["pattern"] = (int)index;
	return json;
}

static PatternData* PatternDataFromJSON(Json::Value json) {
	PatternData* patternData = new PatternData();
	patternData->duration = json["duration"].asInt();

	int index = json["pattern"].asInt();
	patternData->pattern = Patterns::m_Patterns[index];
	return patternData;
}

static int GetPatternMaxProgress(Pattern* pattern) {
	int time = 0;
	for (PatternStep* step : pattern->steps) time += step->time;
	return time;
}


static void GetPatternStepAndTime(Pattern* pattern, int time, int& outTime, int& outStep) {
	int maxTime = GetPatternMaxProgress(pattern);

	static char buffer[512];

	int t = (int)((float)time - (floor((float)time / (float)maxTime) * (float)maxTime));

	//sprintf(buffer, "time %d; t %d; max %d", time, t, maxTime);
	//MessageBox(HWND_DESKTOP, buffer, "", MB_ICONERROR);

	outTime = 0;

	int findTime = 0;
	int stepIndex = -1;
	while (findTime < t) //&& stepIndex < pattern->steps.size()
	{
		stepIndex++;
		outTime = t - findTime;


		sprintf(buffer, "time %d; t %d; findTime %d; stepIndex %d", time, t, findTime, stepIndex);
		//MessageBox(HWND_DESKTOP, buffer, "", MB_ICONERROR);

		findTime += pattern->steps[stepIndex]->time;

	}

	if (stepIndex == -1) stepIndex = 0;

	outStep = stepIndex;
}
