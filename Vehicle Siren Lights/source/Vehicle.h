#pragma once

#include "pch.h"
#include "Mod.h"

class Vehicle {
public:
	CVehicle* m_Vehicle;

	Vehicle(CVehicle* veh);

	void Update();
	void Draw();
	void Destroy();

	void RenderBefore();
	void RenderAfter();

	void RegisterCoronas();
};