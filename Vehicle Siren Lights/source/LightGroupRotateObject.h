#pragma once

#include "pch.h"

enum class eRotateObjectAxis {
	X,
	Y,
	Z
};

struct LightGroupRotateObject {
	bool rotate = false;
	bool rotateAlways = false;
	float speed = 1.0f;
	eRotateObjectAxis axis = eRotateObjectAxis::Z;
};
