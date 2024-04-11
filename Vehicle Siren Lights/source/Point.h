#pragma once

#include "pch.h"

#include "LightGroupShadow.h"
#include "LightGroupRotateObject.h"

enum class eSirenDirection {
	FRONT,
	BOTH,
	BACK
};

class Point {
public:
	std::string name = "";
	CVector position = CVector(0, 0, 0);
	std::string object = "";

	bool useCustomColor = false;
	CRGBA customColor = CRGBA(255, 255, 0);

	CRGBA disabledColor = CRGBA(0, 0, 0, 255);
	LightGroupShadow shadow;
	LightGroupRotateObject rotateObject;

	/*
	CRGBA GetColor(PatternStep* step) {
		switch (sirenPosition)
		{
		case eSirenPosition::LEFT:
			return step->leftColor;
		case eSirenPosition::MIDDLE:
			return step->middleColor;
		case eSirenPosition::RIGHT:
			return step->rightColor;
		}
		return NULL;
	}
	*/

	bool GetIsEnabled(PatternStep* step, int index) {
		if (index >= (int)step->values.size()) return false;
		return step->values[index] == 1;
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;

		value["name"] = name;
		value["position"] = CVectorToJSON(position);
		value["object"] = object;

		value["useCustomColor"] = useCustomColor;
		value["customColor"] = ColorToJSON(customColor);

		value["disabledColor"] = ColorToJSON(disabledColor);
		//value["sirenPosition"] = (int)sirenPosition;

		Json::Value shadowValue = Json::objectValue;
		shadowValue["enabled"] = shadow.enabled;
		shadowValue["textureIndex"] = shadow.textureIndex;
		shadowValue["position"] = CVectorToJSON(shadow.position);
		shadowValue["angle"] = shadow.angle;
		shadowValue["width"] = shadow.width;
		shadowValue["height"] = shadow.height;
		shadowValue["intensity"] = shadow.intensity;
		shadowValue["rotate"] = shadow.rotate;
		shadowValue["rotateOffsetPos"] = shadow.rotateOffsetPos;
		shadowValue["rotateOffsetAngle"] = shadow.rotateOffsetAngle;
		value["shadow"] = shadowValue;

		Json::Value rotateObjectValue = Json::objectValue;
		rotateObjectValue["rotate"] = rotateObject.rotate;
		rotateObjectValue["rotateAlways"] = rotateObject.rotateAlways;
		rotateObjectValue["speed"] = rotateObject.speed;
		rotateObjectValue["axis"] = (int)rotateObject.axis;
		value["rotateObject"] = rotateObjectValue;

		return value;
	}

	void FromJSON(Json::Value value) {
		name = value["name"].asString();
		position = CVectorFromJSON(value["position"]);
		object = value["object"].asString();

		useCustomColor = ValidateValue(value["useCustomColor"], useCustomColor).asBool();;
		customColor = ValidateColor(value["customColor"], customColor);

		disabledColor = ColorFromJSON(value["disabledColor"]);
		//sirenPosition = (eSirenPosition)value["sirenPosition"].asInt();

		Json::Value shadowValue = value["shadow"];
		if (!shadowValue.isNull())
		{
			shadow.enabled = shadowValue["enabled"].asBool();
			shadow.textureIndex = shadowValue["textureIndex"].asInt();
			shadow.position = CVectorFromJSON(shadowValue["position"]);
			shadow.angle = shadowValue["angle"].asFloat();
			shadow.width = shadowValue["width"].asFloat();
			shadow.height = shadowValue["height"].asFloat();
			shadow.intensity = ValidateValue(shadowValue["intensity"], shadow.intensity).asFloat();

			shadow.rotate = ValidateValue(shadowValue["rotate"], shadow.rotate).asBool();
			shadow.rotateOffsetPos = ValidateValue(shadowValue["rotateOffsetPos"], shadow.rotateOffsetPos).asFloat();
			shadow.rotateOffsetAngle = ValidateValue(shadowValue["rotateOffsetAngle"], shadow.rotateOffsetAngle).asFloat();
		}

		Json::Value rotateObjectValue = value["rotateObject"];
		if (!rotateObjectValue.isNull())
		{
			rotateObject.rotate = rotateObjectValue["rotate"].asBool();
			rotateObject.rotateAlways = rotateObjectValue["rotateAlways"].asBool();
			rotateObject.speed = rotateObjectValue["speed"].asFloat();
			rotateObject.axis = (eRotateObjectAxis)rotateObjectValue["axis"].asInt();
		}
	}

	static double GetAngle(CVehicle* vehicle, CVector position) {
		double angle = GetAngleBetweenVectors(vehicle->TransformFromObjectSpace(CVector(position.x, 0, position.z)), vehicle->TransformFromObjectSpace(position), TheCamera.GetPosition());
		if (isnan(angle)) {
			angle = 0.0001;
		}
		return angle;
	}

	static double GetRadiusMultiplierByAngle(double currentDir, double startFadeDir, double offsetFadeDir) {
		double end = startFadeDir + offsetFadeDir;

		if (offsetFadeDir < 0) {
			if (currentDir < startFadeDir + offsetFadeDir) currentDir = startFadeDir + offsetFadeDir;
			if (currentDir > startFadeDir) currentDir = startFadeDir;
		}
		else {
			if (currentDir < startFadeDir) currentDir = startFadeDir;
			if (currentDir > startFadeDir + offsetFadeDir) currentDir = startFadeDir + offsetFadeDir;
		}

		return abs((currentDir - end) / offsetFadeDir);
	}

	float GetRadiusMultiplier(double angle, eSirenDirection direction, float positionY) {
		if (direction == eSirenDirection::BOTH) return 1.0f;

		eSirenDirection atDirection = positionY > 0 ? eSirenDirection::FRONT : eSirenDirection::BACK;

		double start_fadeout = 1.0f;
		double fadeout_offset = 0.6f;

		if (atDirection != direction) {
			start_fadeout += fadeout_offset;
			fadeout_offset *= -1;
		}

		return (float)GetRadiusMultiplierByAngle(angle, start_fadeout, fadeout_offset);
	}
};