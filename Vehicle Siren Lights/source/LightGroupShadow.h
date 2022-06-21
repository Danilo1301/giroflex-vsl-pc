#pragma once

#include "pch.h"

struct LightGroupShadow {
	bool enabled = false;
	int textureIndex = 0;
	CVector position = CVector(0, 0, 0);
	float angle = 0.0f;
	float width = 3.0f;
	float height = 3.0f;
	float intensity = 1.0f;
	bool rotate = false;
	float rotateOffsetPos = 0.0f;
	float rotateOffsetAngle = 0.0f;
};

class LightGroupShadows {
public:
	static const int NUM_OF_SHADOWS = 6;
	static std::vector<RwTexture*> m_ShadowTextures;

	static void LoadTextures();
};