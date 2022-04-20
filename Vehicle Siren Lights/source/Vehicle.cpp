#include "Vehicle.h"
#include "VehicleDummy.h"
#include "LightGroups.h"
#include "CVisibilityPlugins.h"

Vehicle::Vehicle(CVehicle* veh) {
	m_Vehicle = veh;	

	CheckForLightGroups();
}

void Vehicle::Update() {
	CheckForLightGroups();
	UpdatePatternAndSteps();
	RegisterCoronas();
}

void Vehicle::Draw() {
}

void Vehicle::DrawDebug() {
	char text[512];
	auto screenPos = WorldToScreen(m_Vehicle->GetPosition() + CVector(0, 0, -0.5f));

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
}

void Vehicle::DrawFrames() {
	char text[512];

	auto frames = VehicleDummy::GetFramesOnVehicle(m_Vehicle);
	for (auto frame : frames) {
		if (frame == m_Vehicle->m_pRwClump->object.parent) continue;

		std::string name = GetFrameNodeName(frame);

		//if (name.compare("bau") != 0) return;

		CVector position = m_Vehicle->TransformFromObjectSpace(GetFrameNodePosition(frame));

		sprintf(text, "%s (%d)", name.c_str(), name.compare("bau"));
		DrawWorldText(text, position);
		
	}
}

void Vehicle::DrawPoints() {
	char text[512];

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		int i = 0;
		for (auto point : lightGroup->points) {
			auto position = m_Vehicle->TransformFromObjectSpace(lightGroup->position + point->position);

			sprintf(text, "Point %d", i + 1);
			DrawWorldText(text, position);

			i++;
		}
	}
}

void Vehicle::Destroy() {

}

void Vehicle::RenderBefore() {
	//char buffer[512];

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

					//if (!vehiclePatternData->enabled) continue;

					auto patternCycleStep = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex];
					auto patternDuration = patternCycleStep->duration;
					auto pattern = patternCycleStep->pattern;

					auto step = pattern->steps[vehiclePatternData->stepLoop->m_StepIndex];

					//lightId += lightGroup->offsetId;

					for (auto point : lightGroup->points) {
						if (name.compare(point->object) == 0) {

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
							return material;
							}, 0);

							//

							for (auto material : materials) {
								CRGBA color = point->GetColor(step);
								bool enabled = point->GetIsEnabled(step);

								if (enabled) {
									material->color = { color.r, color.g, color.b, color.a };
								}
								else {
									material->color = { 0, 0, 0, 255 };
								}

								material->surfaceProps.ambient = 100.0f;
								material->surfaceProps.diffuse = 100.0f;
								material->surfaceProps.specular = 100.0f;
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

}

void Vehicle::UpdateVehicleMaterial(RpMaterial* material, std::string frameName) {

}

void Vehicle::RegisterCorona(int lightid, CVector position, CRGBA color, float radius) {
	CCoronas::RegisterCorona(
		lightid,
		m_Vehicle,
		color.r,
		color.g,
		color.b,
		color.a,
		position,
		radius,
		500.0f,
		eCoronaType::CORONATYPE_SHINYSTAR,
		eCoronaFlareType::FLARETYPE_NONE,
		false,
		false,
		0,
		0.0f,
		false,
		0.1f,
		0,
		100.0f,
		false,
		false
	);
}

void Vehicle::CheckForLightGroups() {
	if (LightGroups::HasLightGroups(m_Vehicle->m_nModelIndex)) {
		auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);
		for (auto lightGroup : lightGroups) {
			if (!m_LightGroupData[lightGroup]) {
				auto vehiclePatternData = new VehiclePatternData();

				Log::file << "Adding " << lightGroup->patternCycleSteps.size() << " cycle steps" << std::endl;

				for (auto patternCycleStep : lightGroup->patternCycleSteps) {
					vehiclePatternData->patternLoop->AddStep(&patternCycleStep->duration);
				}

				m_LightGroupData[lightGroup] = vehiclePatternData;

				Log::file << "[Vehicle " << m_Vehicle << "] Added vehiclePatternData for LightGroup (" << std::to_string(m_LightGroupData.size()) << " total)" << std::endl;
			}
		}
	}
}

void Vehicle::UpdatePatternAndSteps() {
	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		int prevPatternIndex = vehiclePatternData->patternLoop->m_StepIndex;
		vehiclePatternData->patternLoop->Update(10);
		bool patternChanged = prevPatternIndex != vehiclePatternData->patternLoop->m_StepIndex;

		if (patternChanged || vehiclePatternData->stepLoop->GetSteps().size() == 0) {
			vehiclePatternData->stepLoop->Clear();

			auto pattern = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex]->pattern;

			for (size_t i = 0; i < pattern->steps.size(); i++) {
				vehiclePatternData->stepLoop->AddStep(&pattern->steps[i]->duration);;
			}
		}

		vehiclePatternData->stepLoop->Update(10);
	}
}

void Vehicle::RegisterCoronas() {
	int lightId = reinterpret_cast<unsigned int>(m_Vehicle) + 20;

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		if (!vehiclePatternData->enabled) continue;

		auto patternCycleStep = lightGroup->patternCycleSteps[vehiclePatternData->patternLoop->m_StepIndex];
		auto patternDuration = patternCycleStep->duration;
		auto pattern = patternCycleStep->pattern;

		auto step = pattern->steps[vehiclePatternData->stepLoop->m_StepIndex];

		//lightId += lightGroup->offsetId;

		for (auto point : lightGroup->points) {

			CVector position = point->position;

			CRGBA color = point->GetColor(step);
			bool enabled = point->GetIsEnabled(step);

			RegisterCorona(lightId++, lightGroup->position + position, color, enabled ? lightGroup->size : 0.0f);

			if (lightGroup->reflect && enabled) {
				CVector fposition = m_Vehicle->TransformFromObjectSpace(lightGroup->position + position);
				Command< 0x09E5 >(fposition.x, fposition.y, fposition.z, (int)color.r, (int)color.g, (int)color.b, lightGroup->reflectionDistance);
			}
		}
	}
}