#include "Vehicle.h"
#include "VehicleDummy.h"
#include "LightGroups.h"
#include "menu/Menu.h"

#include "windows/WindowLightGroup.h"

#include "CVisibilityPlugins.h"
#include "CShadows.h"

float Vehicle::m_MatAmbient = 2.5f;
bool Vehicle::m_FreezeLights = false;

static std::list<std::pair<unsigned int*, unsigned int>> m_ResetEntries;

Vehicle::Vehicle(CVehicle* veh) {
	m_Vehicle = veh;	

	CheckForLightGroups();

	m_PrevTime = CTimer::m_snTimeInMilliseconds;
}

void Vehicle::Update() {
	CheckForLightGroups();
	UpdatePatternAndSteps();
	UpdateSirenState();
	RegisterCoronas();

	m_PrevTime = CTimer::m_snTimeInMilliseconds;
}

void Vehicle::UpdateSirenState() {
	//Log::file << "[Vehicle " << m_Vehicle << "] UpdateSirenState" << std::endl;

	bool currentSirenState = GetSirenState();

	if (currentSirenState != m_PrevSirenState)
	{
		m_PrevSirenState = currentSirenState;
		SetAllLightGroupState(currentSirenState);
	}
}

void Vehicle::Draw() {
}

void Vehicle::DrawDebug() {
	char text[512];
	auto screenPos = WorldToScreen(m_Vehicle->GetPosition() + CVector(0, 0, -0.5f));

	//draw angle
	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		for (auto point : lightGroup->points) {
			auto position = m_Vehicle->TransformFromObjectSpace(lightGroup->position + point->position);

			auto positionY = lightGroup->position.y + point->position.y;

			auto angle = Point::GetAngle(m_Vehicle, lightGroup->position + point->position);
			auto radiusMult = point->GetRadiusMultiplier(angle, lightGroup->direction, positionY);

			eSirenDirection atDirection = positionY > 0 ? eSirenDirection::FRONT : eSirenDirection::BACK;

			sprintf(text, "%.2f x%.1f [atDir= %d] [dir= %d]", (float)angle, radiusMult, (int)atDirection, (int)lightGroup->direction);
			DrawWorldText(text, position);
		}
	}

	//draw pattern and step info
	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		sprintf(text, "[lightGroup] patternLoop %d (%d)  -  stepLoop %d (%d)", vehiclePatternData->patternLoop->m_StepIndex, vehiclePatternData->patternLoop->GetSteps().size(), vehiclePatternData->stepLoop->m_StepIndex, vehiclePatternData->stepLoop->GetSteps().size());
		DrawScreenText(text, screenPos);
		screenPos.y += 20.0f;

		sprintf(text, "[lightGroup] %d points", lightGroup->points.size());
		DrawScreenText(text, screenPos);
		screenPos.y += 20.0f;
	}

	sprintf(text, "[time] %d %d", CTimer::m_snTimeInMilliseconds, m_PrevTime);
	DrawScreenText(text, screenPos);
	screenPos.y += 20.0f;
}

void Vehicle::DrawFrames() {
	char text[512];

	RwFrame* froot = (RwFrame*)m_Vehicle->m_pRwClump->object.parent;

	auto frames = VehicleDummy::GetFramesOnVehicle(m_Vehicle);
	for (auto frame : frames) {
		if (frame == froot) continue;

		CVector position = VehicleDummy::GetTransformedDummyPosition(m_Vehicle, frame, CVector(0, 0, 0));

		std::string name = GetFrameNodeName(frame);

		sprintf(text, "%s", name.c_str());
		DrawWorldText(text, position);
	}
}

void Vehicle::DrawPoints() {
	char text[512];

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		if (lightGroup != WindowLightGroup::m_LightGroup) continue;

		int i = 0;
		for (auto point : lightGroup->points) {
			auto position = VehicleDummy::GetTransformedPosition(m_Vehicle, lightGroup->position + point->position);

			sprintf(text, "(%d)", i + 1);
			DrawWorldText(text, position);

			i++;
		}
	}
}

void Vehicle::Destroy() {

}

bool Vehicle::GetSirenState() {
	if (Menu::m_Visible && FindPlayerVehicle(0, false) == m_Vehicle) return true;
	return m_Vehicle->m_nVehicleFlags.bSirenOrAlarm;
}

void Vehicle::RenderBefore() {
	//char buffer[512];

	if (m_LightGroupData.size() == 0) return;

	static CVehicle* s_vehicle;
	s_vehicle = m_Vehicle;

	static auto lightGroupData = m_LightGroupData;
	lightGroupData = m_LightGroupData;

	if (m_Vehicle->m_pRwClump && m_Vehicle->m_pRwClump->object.type == rpCLUMP) {
		RpClumpForAllAtomics(m_Vehicle->m_pRwClump, [](RpAtomic* atomic, void* data) {
			if (atomic->geometry) {
				atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

				RwFrame* frameAtomic = GetObjectParent((RwObject*)atomic);
				std::string name = GetFrameNodeName(frameAtomic);
				CVector position = s_vehicle->TransformFromObjectSpace(GetFrameNodePosition(frameAtomic));

				//

				for (auto pair : lightGroupData) {
					auto lightGroup = pair.first;
					auto vehiclePatternData = pair.second;

					auto patternCycleStep = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex];
					auto patternDuration = patternCycleStep->duration;
					auto pattern = patternCycleStep->pattern;

					auto step = pattern->steps[vehiclePatternData->stepLoop->m_StepIndex];

					for (auto point : lightGroup->points) {
						if (StringToUpper(name).compare(StringToUpper(point->object)) == 0) {

							/*
							char text[512];
							sprintf(text, "%s", name.c_str());
							DrawWorldText(text, position, CRGBA(255, 0, 0));
							*/

							//

							static std::vector<RpMaterial*> materials;
							materials.clear();
							RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial* material, void* data) {
								materials.push_back(material);

								//m_ResetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&material->color), *reinterpret_cast<unsigned int*>(&material->color)));
								//m_ResetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&material->surfaceProps.ambient), *reinterpret_cast<unsigned int*>(&material->surfaceProps.ambient)));

							return material;
							}, 0);

							//

							if (!m_FreezeLights) {
								if (!vehiclePatternData->enabled) {
									for (auto material : materials) {
										material->color = { point->disabledColor.r, point->disabledColor.g, point->disabledColor.b, point->disabledColor.a };
									}
									continue;
								}
							}

							//

							for (auto material : materials) {
								CRGBA color = lightGroup->usePatternColors ? point->GetColor(step) : point->color;
								bool enabled = point->GetIsEnabled(step);

								if (m_FreezeLights) enabled = true;

								if (enabled) {
									material->color = { color.r, color.g, color.b, color.a };
								}
								else {
									material->color = { point->disabledColor.r, point->disabledColor.g, point->disabledColor.b, point->disabledColor.a };
								}

								material->surfaceProps.ambient = m_MatAmbient;

								
								//material->surfaceProps.diffuse = m_MatDiffuse;
								//material->surfaceProps.specular = m_MatSpecular;
							}

							//

							/*
							sprintf(text, "");
							for (auto material : materials) {
								sprintf(text, "%s %s,", text, "MATERIAL");
							}
							DrawWorldText(text, position, CRGBA(0, 0, 255));
							*/
						}
					}
				}
			}
		return atomic;
		}, 0);
	}

}

void Vehicle::RenderAfter() {
	for (auto& p : m_ResetEntries) *p.first = p.second;
	m_ResetEntries.clear();

	RenderShadows();
}

void Vehicle::RenderShadows() {
	int shadowId = (uintptr_t)m_Vehicle + 100;
	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		auto patternCycleStep = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex];
		auto patternDuration = patternCycleStep->duration;
		auto pattern = patternCycleStep->pattern;

		auto step = pattern->steps[vehiclePatternData->stepLoop->m_StepIndex];

		
		for (auto point : lightGroup->points) {
			if (!point->shadow.enabled) continue;

			//auto pos = VehicleDummy::GetTransformedPosition(m_Vehicle, lightGroup->position + point->position + CVector(point->shadow.position.x, point->shadow.position.y, 0));
			CVector pos = m_Vehicle->GetPosition();

			CVector2D fwd(*GetForward(m_Vehicle));
			CVector2D_Normalize(&fwd);
			CVector2D_Rotate(&fwd, point->shadow.angle);

			auto offsetPos = lightGroup->position + point->position + point->shadow.position;

			float f = offsetPos.y + 0.0f;
			pos += CVector(f * fwd.x, f * fwd.y, 2.0f);

			float width = point->shadow.width;
			float height = point->shadow.height;

			//

			CRGBA color = lightGroup->usePatternColors ? point->GetColor(step) : point->color;
			bool enabled = point->GetIsEnabled(step);

			if (!m_FreezeLights) {
				if (!vehiclePatternData->enabled) enabled = false;
			}
			else { enabled = true; }

			//

			if (!enabled) continue;

			CShadows::StoreCarLightShadow(
				m_Vehicle,
				shadowId++,
				LightGroupShadows::m_ShadowTextures[point->shadow.textureIndex], // gpShadowExplosionTex, //LightGroupShadows::m_ShadowTexture,
				&pos,
				height * fwd.x,
				height * fwd.y,
				width * fwd.y,
				-width * fwd.x,
				color.r,
				color.g,
				color.b,
				7.0f
			);
		}
	}
}

void Vehicle::UpdateVehicleMaterial(RpMaterial* material, std::string frameName) {

}

void Vehicle::SetAllLightGroupState(bool enabled) {
	//Log::file << "[Vehicle " << m_Vehicle << "] SetAllLightGroupState" << std::endl;

	m_PrevLightState = enabled;

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		vehiclePatternData->patternLoop->Reset();
		vehiclePatternData->stepLoop->Clear();
		vehiclePatternData->stepLoop->Reset();

		if (lightGroup->turnOnSiren) {
			vehiclePatternData->enabled = enabled;
		}
	}
}

void Vehicle::CheckForLightGroups() {
	//Log::file << "[Vehicle " << m_Vehicle << "] CheckForLightGroups" << std::endl;

	if (LightGroups::HasLightGroups(m_Vehicle->m_nModelIndex)) {
		auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);
		for (auto lightGroup : lightGroups) {
			if (!m_LightGroupData[lightGroup]) {
				if (lightGroup->patternCycleSteps.size() == 0) {
					m_LightGroupData.erase(lightGroup);
					continue;
				}

				Log::file << "[Vehicle " << m_Vehicle << "] Add VehiclePatternData for light group '" << lightGroup->name << "' (" << std::to_string(m_LightGroupData.size()) << " total) with " << lightGroup->patternCycleSteps.size() << " patterns" << std::endl;

				auto vehiclePatternData = new VehiclePatternData();

				if (lightGroup->turnOnSiren) vehiclePatternData->enabled = GetSirenState();

				for (auto patternCycleStep : lightGroup->patternCycleSteps) {
					vehiclePatternData->patternLoop->AddStep(&patternCycleStep->duration);
				}

				m_LightGroupData[lightGroup] = vehiclePatternData;
			
			}
		}
	}

	//Log::file << "[Vehicle " << m_Vehicle << "] CheckForLightGroups, found" << m_LightGroupData.size() << std::endl;
}

void Vehicle::UpdatePatternAndSteps() {
	//Log::file << "[Vehicle " << m_Vehicle << "] UpdatePatternAndSteps " << m_LightGroupData.size() << " light groups" << std::endl;

	int updateMs = CTimer::m_snTimeInMilliseconds - m_PrevTime;

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		if (!vehiclePatternData->enabled) continue;

		int prevPatternIndex = vehiclePatternData->patternLoop->m_StepIndex;
		vehiclePatternData->patternLoop->Update(updateMs);
		bool patternChanged = prevPatternIndex != vehiclePatternData->patternLoop->m_StepIndex;

		

		if (patternChanged || vehiclePatternData->stepLoop->GetSteps().size() == 0) {
			if (vehiclePatternData->stepLoop->GetSteps().size() > 0) {
				//Log::file << "[Vehicle] patternChanged patternLoop=" << vehiclePatternData->patternLoop->m_Time << "(" << updateMs << ")" << std::endl;
				//Log::file << "[Vehicle] stepLoop= " << vehiclePatternData->stepLoop->m_Time << " will be " << (vehiclePatternData->stepLoop->m_Time + updateMs) << std::endl;
			}

			vehiclePatternData->stepLoop->Clear();
			vehiclePatternData->stepLoop->Reset();

			int newTime = vehiclePatternData->patternLoop->m_Time - updateMs;

			vehiclePatternData->stepLoop->m_Time = newTime;
			vehiclePatternData->stepLoop->m_TotalTime = newTime;

			auto pattern = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex]->pattern;

			for (size_t i = 0; i < pattern->steps.size(); i++) {
				vehiclePatternData->stepLoop->AddStep(&pattern->steps[i]->duration);;
			}

			if (vehiclePatternData->stepLoop->GetSteps().size() > 0) {
				//Log::file << "[Vehicle] patternLoop= " << vehiclePatternData->patternLoop->m_Time << " | " << (vehiclePatternData->patternLoop->m_TotalTime) << std::endl;
				//Log::file << "[Vehicle] stepLoop= " << vehiclePatternData->stepLoop->m_Time << " | " << (vehiclePatternData->stepLoop->m_TotalTime) << std::endl;
			}
			//Log::file << "||" << std::endl;
		}

		

		vehiclePatternData->stepLoop->Update(updateMs);
	}
}

void Vehicle::RegisterCoronas() {
	//Log::file << "[Vehicle " << m_Vehicle << "] RegisterCoronas" << std::endl;

	int lightId = reinterpret_cast<unsigned int>(m_Vehicle) + 50;

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		lightId += lightGroup->offsetId;

		auto patternCycleStep = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex];
		auto patternDuration = patternCycleStep->duration;
		auto pattern = patternCycleStep->pattern;

		auto step = pattern->steps[vehiclePatternData->stepLoop->m_StepIndex];

		for (auto point : lightGroup->points) {

			CVector position = point->position;

			CRGBA color = lightGroup->usePatternColors ? point->GetColor(step) : point->color;
			bool enabled = point->GetIsEnabled(step);

			if (!m_FreezeLights) {
				if (!vehiclePatternData->enabled) enabled = false;
			}
			else { enabled = true; }

			//

			auto positionY = lightGroup->position.y + point->position.y;
			auto angle = Point::GetAngle(m_Vehicle, lightGroup->position + point->position);
			auto radiusMult = point->GetRadiusMultiplier(angle, lightGroup->direction, positionY);
			auto radius = lightGroup->size * radiusMult;

			//

			CCoronas::RegisterCorona(
				lightId++,
				m_Vehicle,
				color.r,
				color.g,
				color.b,
				color.a,
				lightGroup->position + position,
				enabled ? radius : 0.0f,
				800.0f,
				lightGroup->type,
				enabled ? lightGroup->flareType : eCoronaFlareType::FLARETYPE_NONE,
				false, //lightGroup->cReflect,
				false,
				0,
				0.0f,
				false,
				lightGroup->nearClip,
				0,
				100.0f,
				false,
				false
			);

			if (lightGroup->reflect && enabled) {
				CVector fposition = m_Vehicle->TransformFromObjectSpace(lightGroup->position + position);
				Command< 0x09E5 >(fposition.x, fposition.y, fposition.z, (int)color.r, (int)color.g, (int)color.b, lightGroup->reflectionDistance);
			}
		}
	}
}