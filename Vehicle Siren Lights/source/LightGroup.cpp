#include "LightGroup.h"

#include "TestHelper.h"
#include "Mod.h"
#include "Config.h"

#include "log/Log.h"

LightGroup::LightGroup(int modelId)
{
	this->modelId = modelId;
	this->keybindMenu.SaveAsNumber = true;

	std::string path = Config::GetFullPluginPath(Config::m_DataPath + Config::m_VehiclesPath + std::to_string(modelId));
	this->modelDirectoryEntry = std::filesystem::directory_entry(std::filesystem::path(path));
	Log::file << "[LightGroup] Default modelDirectoryEntry: " << modelDirectoryEntry.path().string() << std::endl;
}

Point* LightGroup::AddPoint()
{
	Point* point = new Point();
	points.push_back(point);
	return point;
}

Point* LightGroup::AddPoint(eSirenPosition sirenPosition)
{
	Point* point = AddPoint();
	point->sirenPosition = sirenPosition;
	return point;
}

void LightGroup::RemovePoint(Point* point)
{
	auto it = std::find(points.begin(), points.end(), point);
	if (it == points.end()) return;
	points.erase(it);
	delete point;
}

void LightGroup::SetNumberOfPoints(int amount)
{
	while ((int)points.size() < amount)
	{
		if (isPositionLightGroup)
		{
			AddPoint(eSirenPosition::RIGHT);
		}
		else {
			AddPoint();
		}
	}
	while ((int)points.size() > amount)
	{
		RemovePoint(points[points.size() - 1]);
	}
}


void LightGroup::AddPatternCycleStep(Pattern* pattern, int duration)
{
	PatternCycleStep* patternCycleStep = new PatternCycleStep();
	patternCycleStep->duration = duration;
	patternCycleStep->lerpColor = false;
	patternCycleStep->pattern = pattern;
	patternCycleSteps.push_back(patternCycleStep);
}

void LightGroup::RemovePatternCycleStep(PatternCycleStep* patternCycleStep)
{
	Log::file << "[LightGroup : " << name << "] Pattern '" << patternCycleStep->pattern->name << "' removed" << std::endl;

	auto it = std::find(patternCycleSteps.begin(), patternCycleSteps.end(), patternCycleStep);
	if (it == patternCycleSteps.end()) return;
	patternCycleSteps.erase(it);
	delete patternCycleStep;
}

void LightGroup::RemoveAllPatternCycleSteps()
{
	while (patternCycleSteps.size() > 0)
		RemovePatternCycleStep(patternCycleSteps[0]);
}

void LightGroup::FindNewPatterns()
{
	RemoveAllPatternCycleSteps();

	if (isPositionLightGroup)
	{
		for (auto pattern : Patterns::m_Patterns)
		{
			if (pattern->steps[0]->values.size() == 3)
				AddPatternCycleStep(pattern, 7000);
		}
	}
	else {
		for (auto pattern : Patterns::m_Patterns)
		{
			if (pattern->steps[0]->values.size() == points.size())
				AddPatternCycleStep(pattern, 7000);
		}
	}
}

void LightGroup::Destroy()
{
	RemoveAllPatternCycleSteps();

	RemoveAllPoints();
}

void LightGroup::RemoveAllPoints()
{
	while (points.size() > 0)
		RemovePoint(points[0]);
}

Json::Value LightGroup::ToJSON()
{
	Json::Value value = Json::objectValue;

	value["lightbarSettings"] = Json::objectValue;
	value["lightbarSettings"]["useLightbarLeds"] = lightbarSettings.useLightbarLeds;
	value["lightbarSettings"]["object_prefix"] = lightbarSettings.object_prefix;
	value["lightbarSettings"]["ledOnColor"] = ColorToJSON(lightbarSettings.ledOnColor);
	value["lightbarSettings"]["ledOffColor"] = ColorToJSON(lightbarSettings.ledOffColor);

	value["keybindMenu"] = keybindMenu.ToJSON();

	value["name"] = name;

	value["isPositionLightGroup"] = isPositionLightGroup;

	value["color1"] = ColorToJSON(color1);
	value["color2"] = ColorToJSON(color2);
	value["color3"] = ColorToJSON(color3);

	value["reflect"] = reflect;
	value["reflectionIntensity"] = reflectionIntensity;
	value["reflectionDistance"] = reflectionDistance;
	value["turnOnSiren"] = turnOnSiren;
	value["size"] = size;
	value["distance"] = distance;
	value["curve"] = curve;
	value["nearClip"] = nearClip;
	value["flareType"] = (int)flareType;
	value["flareDistance"] = flareDistance;
	value["flareIntensity"] = flareIntensity;
	value["type"] = (int)type;
	value["direction"] = (int)direction;
	value["usePatternColors"] = usePatternColors;
	value["position"] = CVectorToJSON(position);

	value["points"] = Json::arrayValue;
	for (auto point : points) {
		value["points"].append(point->ToJSON());
	}

	value["pointsPositionFixed"] = pointsPositionFixed;

	value["patternCycleSteps"] = Json::arrayValue;
	for (auto patternCycleStep : patternCycleSteps) {
		value["patternCycleSteps"].append(PatternCycleStepToJSON(patternCycleStep));
	}

	value["patternOffset"] = patternOffset;
	value["useSmallWhiteCorona"] = useSmallWhiteCorona;
	value["smallWhiteCoronaScale"] = smallWhiteCoronaScale;
	value["smallWhiteCoronaType"] = (int)smallWhiteCoronaType;
	value["freezeLights"] = freezeLights;
	value["alwaysEnabled"] = alwaysEnabled;

	value["version"] = Mod::m_Version;

	return value;
}

void LightGroup::FromJSON(Json::Value value)
{
	name = ValidateValue(value["name"], name).asString();

	if (!value["lightbarSettings"].isNull())
	{
		lightbarSettings.useLightbarLeds = ValidateValue(value["lightbarSettings"]["useLightbarLeds"], lightbarSettings.useLightbarLeds).asBool();
		lightbarSettings.object_prefix = ValidateValue(value["lightbarSettings"]["object_prefix"], lightbarSettings.object_prefix).asString();
		lightbarSettings.ledOnColor = ColorFromJSON(value["lightbarSettings"]["ledOnColor"]);
		lightbarSettings.ledOffColor = ColorFromJSON(value["lightbarSettings"]["ledOffColor"]);
	}

	if (!value["keybindMenu"].isNull()) keybindMenu.FromJSON(value["keybindMenu"]);

	isPositionLightGroup = ValidateValue(value["isPositionLightGroup"], isPositionLightGroup).asBool();

	color1 = ValidateColor(value["color1"], color1);
	color2 = ValidateColor(value["color2"], color2);
	color3 = ValidateColor(value["color3"], color3);

	reflect = ValidateValue(value["reflect"], reflect).asBool();
	reflectionIntensity = ValidateValue(value["reflectionIntensity"], reflectionIntensity).asFloat();
	reflectionDistance = ValidateValue(value["reflectionDistance"], reflectionDistance).asFloat();
	turnOnSiren = ValidateValue(value["turnOnSiren"], turnOnSiren).asBool();
	size = ValidateValue(value["size"], size).asFloat();
	distance = ValidateValue(value["distance"], distance).asFloat();
	curve = ValidateValue(value["curve"], curve).asFloat();
	nearClip = ValidateValue(value["nearClip"], nearClip).asFloat();
	flareType = (eCoronaFlareType)ValidateValue(value["flareType"], (int)flareType).asInt();
	flareDistance = ValidateValue(value["flareDistance"], flareDistance).asFloat();
	flareIntensity = ValidateValue(value["flareIntensity"], flareIntensity).asFloat();
	type = (eCoronaType)ValidateValue(value["type"], (int)type).asInt();
	direction = (eSirenDirection)ValidateValue(value["direction"], (int)direction).asInt();
	usePatternColors = ValidateValue(value["usePatternColors"], usePatternColors).asBool();
	position = ValidateCVector(value["position"], position);

	for (size_t i = 0; i < value["points"].size(); i++)
	{
		Json::Value pointValue = value["points"][i];

		Point* point = new Point();
		point->FromJSON(pointValue);
		points.push_back(point);
	}

	pointsPositionFixed = ValidateValue(value["pointsPositionFixed"], pointsPositionFixed).asBool();

	for (size_t i = 0; i < value["patternCycleSteps"].size(); i++)
	{
		Json::Value patternCycleStepValue = value["patternCycleSteps"][i];

		PatternCycleStep* patternCycleStep = PatternCycleStepFromJSON(patternCycleStepValue);

		if (patternCycleStep->pattern == nullptr) {
			Log::file << "[LightGroup] Pattern not found, patternCycleStep not added" << std::endl;

			delete patternCycleStep;
		}
		else {
			patternCycleSteps.push_back(patternCycleStep);
		}
	}

	patternOffset = ValidateValue(value["patternOffset"], patternOffset).asInt();
	useSmallWhiteCorona = ValidateValue(value["useSmallWhiteCorona"], useSmallWhiteCorona).asBool();
	smallWhiteCoronaScale = ValidateValue(value["smallWhiteCoronaScale"], smallWhiteCoronaScale).asFloat();
	smallWhiteCoronaType = (eCoronaType)ValidateValue(value["smallWhiteCoronaType"], (int)smallWhiteCoronaType).asInt();
	freezeLights = ValidateValue(value["freezeLights"], freezeLights).asBool();
	alwaysEnabled = ValidateValue(value["alwaysEnabled"], alwaysEnabled).asBool();
}