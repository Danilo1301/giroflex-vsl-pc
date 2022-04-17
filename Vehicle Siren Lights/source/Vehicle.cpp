#include "Vehicle.h"
#include "Mod.h"
#include "input/Input.h"

std::list<std::pair<unsigned int*, unsigned int>> m_ResetEntries;

Vehicle::Vehicle(CVehicle* veh) {
	m_Vehicle = veh;
}

void Vehicle::Update() {
	if (ColorTextures::HasColorTextures(m_Vehicle->m_nModelIndex)) {
		auto colorTextures = ColorTextures::GetColorTextures(m_Vehicle->m_nModelIndex);
		for (auto colorTexture : colorTextures) {
			if (colorTexture->patternsData.size() == 0) continue;

			if (!m_ColorTextureData[colorTexture]) {
				//Log::file << "[Vehicle " << m_Vehicle << "] Add colorTextureData (" << std::to_string(m_ColorTextureData.size() + 1) << " total)" << std::endl;

				auto colorTextureData = new ColorTextureData();
				colorTextureData->colorTexture = colorTexture;
				m_ColorTextureData[colorTexture] = colorTextureData;

				if (colorTexture->turnOnSiren) colorTextureData->enabled = false;

				Log::file << "[Vehicle " << m_Vehicle << "] Added colorTextureData (" << std::to_string(m_ColorTextureData.size()) << " total)" << std::endl;
				for (auto a : m_ColorTextureData)
				{
					Log::file << "* " << a.first->name << " with " << std::to_string(a.first->patternsData.size()) << " patterns" << std::endl;
				}
			}
		}
	}

	//

	if (LightGroups::HasLightGroups(m_Vehicle->m_nModelIndex)) {
		auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);
		for (auto lightGroup : lightGroups) {
			if (lightGroup->patternsData.size() == 0) continue;

			if (!m_LightGroupData[lightGroup]) {

				auto lightGroupData = new LightGroupData();
				lightGroupData->lightGroup = lightGroup;
				m_LightGroupData[lightGroup] = lightGroupData;

				if (lightGroup->turnOnSiren) lightGroupData->enabled = false;

				Log::file << "[Vehicle " << m_Vehicle << "] Added lightGroupData (" << std::to_string(m_LightGroupData.size()) << " total)" << std::endl;
				for (auto a : m_ColorTextureData)
				{
					Log::file << "* " << a.first->name << " with " << std::to_string(a.first->patternsData.size()) << " patterns" << std::endl;
				}
			}
		}
	}
	
	//

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		auto lightGroupPattern = GetLightGroupPatternData(lightGroupData);
		auto pattern = lightGroupPattern->pattern;
		auto patternDuration = lightGroupPattern->duration;

		lightGroupData->patternProgress += 10;
		if (lightGroupData->patternProgress >= GetPatternMaxProgress(pattern)) {
			lightGroupData->patternProgress = 0;
		}

		lightGroupData->patternTime += 10;
		if (lightGroupData->patternTime >= patternDuration) {
			lightGroupData->patternTime = 0;
			lightGroupData->patternProgress = 0;

			lightGroupData->patternIndex++;
			if (lightGroupData->patternIndex >= (int)lightGroupData->lightGroup->patternsData.size()) lightGroupData->patternIndex = 0;
		}
	}

	//

	for (auto pair : m_ColorTextureData) {
		auto colorTexture = pair.first;
		auto colorTextureData = pair.second;

		auto colorTexturePattern = GetColorTexturePatternData(colorTextureData);
		auto pattern = colorTexturePattern->pattern;
		auto patternDuration = colorTexturePattern->duration;

		colorTextureData->patternProgress += 10;
		if (colorTextureData->patternProgress >= GetPatternMaxProgress(pattern)) {
			colorTextureData->patternProgress = 0;
		}

		colorTextureData->patternTime += 10;
		if (colorTextureData->patternTime >= patternDuration) {
			colorTextureData->patternTime = 0;
			colorTextureData->patternProgress = 0;

			colorTextureData->patternIndex++;
			if (colorTextureData->patternIndex >= (int)colorTextureData->colorTexture->patternsData.size()) colorTextureData->patternIndex = 0;
		}
	}


	//

	
	bool currentSirenState = m_Vehicle->m_nVehicleFlags.bSirenOrAlarm;
	if (currentSirenState != m_PrevSirenState)
	{
		m_PrevSirenState = m_Vehicle->m_nVehicleFlags.bSirenOrAlarm;

		for (auto pair : m_ColorTextureData) {
			auto colorTextureData = pair.second;

			if (!pair.first->turnOnSiren) continue;
			colorTextureData->enabled = currentSirenState;
			colorTextureData->patternProgress = 0;
			colorTextureData->patternTime = 0;
			colorTextureData->patternIndex = 0;
		}

		for (auto pair : m_LightGroupData) {
			auto lightGroupData = pair.second;

			if (!pair.first->turnOnSiren) continue;
			lightGroupData->enabled = currentSirenState;
			lightGroupData->patternProgress = 0;
			lightGroupData->patternTime = 0;
			lightGroupData->patternIndex = 0;
		}
	}


	if (!Mod::m_MainMenuVisible) {
		for (auto pair : m_LightGroupData) {
			auto lightGroup = pair.first;
			auto lightGroupData = pair.second;

			if (Input::CheckKeyStroke({ lightGroup->keys[0], lightGroup->keys[1], lightGroup->keys[2] })) {
				lightGroupData->enabled = !lightGroupData->enabled;
				lightGroupData->patternProgress = 0;
				lightGroupData->patternTime = 0;
				lightGroupData->patternIndex = 0;
			}
		}

		for (auto pair : m_ColorTextureData) {
			auto colorTexture = pair.first;
			auto colorTextureData = pair.second;

			if (Input::CheckKeyStroke({ colorTexture->keys[0], colorTexture->keys[1], colorTexture->keys[2] })) {
				colorTextureData->enabled = !colorTextureData->enabled;
				colorTextureData->patternProgress = 0;
				colorTextureData->patternTime = 0;
				colorTextureData->patternIndex = 0;
			}
		}
	}

	

	//

	int lightId = reinterpret_cast<unsigned int>(m_Vehicle) + 20;

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		if (!lightGroupData->enabled) continue;

		auto patternData = GetLightGroupPatternData(lightGroupData);
		auto pattern = patternData->pattern;
		auto patternDuration = patternData->duration;

		std::vector<CVector> points;
		for (auto point : lightGroup->points) {
			points.push_back(point->m_Position);
		}

		int amountOfCoronas = lightGroup->amount;
		float totalDistance = TotalDistanceBetweenPoints(points);

		lightId += lightGroup->offsetId;

		for (int i = 0; i < amountOfCoronas; i++) {
			float linePosition = totalDistance / (amountOfCoronas - 1) * i;

			CVector position = PointInLines(points, linePosition);

			int t;
			int stepIndex;
			int offset = (i * lightGroup->offsetBy);

			GetPatternStepAndTime(
				pattern,
				lightGroupData->patternProgress + offset,
				t,
				stepIndex
			);

			PatternStep* step = pattern->steps[stepIndex];
			PatternStep* nextStep = pattern->steps[(stepIndex + 1) % pattern->steps.size()];

			CRGBA color = step->color;

			if (lightGroup->lerpColor) {
				float lerpProgress = (float)t / (float)step->time;

				color = CRGBALerp(
					step->color,
					nextStep->color,
					lerpProgress
				);
			}

			RegisterCorona(lightId++, lightGroup->position + position, color, lightGroup->size);

			if (lightGroup->useFlash) {
				CVector fposition = m_Vehicle->TransformFromObjectSpace(lightGroup->position + position);

				Command< 0x09E5 >(fposition.x, fposition.y, fposition.z, (int)color.r, (int)color.g, (int)color.b, 30.0f);
			}
		}
	}
}

void Vehicle::Draw() {
	char text[256];

	if (!Mod::m_MainMenuVisible) return;

	CVehicle* pVeh = FindPlayerVehicle(0, false);

	if (pVeh != m_Vehicle) return;

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		int i = 0;
		for (auto point : lightGroup->points) {
			auto position = m_Vehicle->TransformFromObjectSpace(lightGroup->position + point->m_Position);

			sprintf(text, "Point %d", i + 1);
			DrawWorldText(text, position);

			i++;
		}
	}
}

void Vehicle::DrawDebug() {
	char text[256];
	auto screenPos = WorldToScreen(m_Vehicle->GetPosition());

	for (auto pair : m_LightGroupData) {
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		//auto lightGroupPattern = GetLightGroupPattern(lightGroupData);
		//auto pattern = lightGroupPattern->pattern;
		//auto patternDuration = lightGroupPattern->duration;

		//sprintf(text, "[lightGroupData] %d - %d / %d - %d / %d", lightGroupData->patternIndex, lightGroupData->patternTime, patternDuration, lightGroupData->patternProgress, GetPatternMaxProgress(pattern));
		
		sprintf(text, "[lightGroupData] %s : %d - %d %d", lightGroupData->enabled ? "ON" : "OFF", lightGroupData->patternIndex, lightGroupData->patternTime, lightGroupData->patternProgress);
		DrawScreenText(text, screenPos);
		screenPos.y += 20.0f;
	}

	//

	for (auto pair : m_ColorTextureData) {
		auto colorTexture = pair.first;
		auto colorTextureData = pair.second;

		//if (colorTexture->patterns.size() > 0) {
		sprintf(text, "[texture] %d - %d - %d", colorTextureData->patternIndex, colorTextureData->patternTime, colorTextureData->patternProgress);
		DrawScreenText(text, screenPos);
		screenPos.y += 20.0f;
		//}
	}
}

void Vehicle::Destroy() {
	Log::file << "[Vehicle " << m_Vehicle << "] Destroy" << std::endl;

	RemoveLightGroupData();
	RemoveColorTextureData();
}

void Vehicle::RenderBefore() {
	auto vehicle = m_Vehicle;

	if (m_ColorTextureData.size() == 0) return;

	static std::map<ColorTexture*, ColorTextureData*> mapColorTextureData;
	mapColorTextureData = m_ColorTextureData;


	if (vehicle->m_pRwClump && vehicle->m_pRwClump->object.type == rpCLUMP) {
		RpClumpForAllAtomics(vehicle->m_pRwClump, [](RpAtomic* atomic, void* data) {
			if (atomic->geometry) {
				atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

				RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial* material, void* data) {
					if (material->texture > 0) {
						std::string textureName = material->texture->name;

						for (auto pair : mapColorTextureData) {
							auto colorTexture = pair.first;
							auto colorTextureData = pair.second;

							if (!colorTextureData->enabled) {
								material->color = { 0, 0, 0, 255 };
								material->surfaceProps.diffuse = 1.0f;
								continue;
							}

							if (colorTexture->texture.compare(textureName) == 0 || colorTexture->texture.length() == 0) {
								auto colorTexturePattern = GetColorTexturePatternData(colorTextureData);
								auto pattern = colorTexturePattern->pattern;
								//auto patternDuration = colorTexturePattern->duration;

								int t;
								int stepIndex;
								

								GetPatternStepAndTime(
									pattern,
									colorTextureData->patternProgress,
									t,
									stepIndex
								);

								PatternStep* step = pattern->steps[stepIndex];
								PatternStep* nextStep = pattern->steps[(stepIndex + 1) % pattern->steps.size()];

								CRGBA color = step->color;

								if (colorTexture->lerpColor) {
									float lerpProgress = (float)t / (float)step->time;

									color = CRGBALerp(
										step->color,
										nextStep->color,
										lerpProgress
									);
								}

								//m_ResetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&material->color), *reinterpret_cast<unsigned int*>(&material->color)));
								material->color = { color.r, color.g, color.b, 255 };
								material->surfaceProps.diffuse = 5.0f;

								//m_ResetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&material->surfaceProps.specular), *reinterpret_cast<unsigned int*>(&material->surfaceProps.specular)));
								
								//material->surfaceProps.ambient = 1.0f;
								//material->surfaceProps.specular = 1.0f;

								break;
							}
						}
					}
					return material;
					}, 0);
			}
			return atomic;
			}, 0);
	}
}

void Vehicle::RenderAfter() {
	for (auto& p : m_ResetEntries) *p.first = p.second;
	m_ResetEntries.clear();
}

void Vehicle::RemoveLightGroupData() {
	Log::file << "[Vehicle " << m_Vehicle << "] RemoveLightGroupData" << std::endl;

	std::vector<LightGroup*> entries;

	for (auto pair : m_LightGroupData) {
		entries.push_back(pair.first);
	}

	for (auto entry : entries) {
		delete m_LightGroupData[entry];
	}

	m_LightGroupData.clear();

}

void Vehicle::RemoveColorTextureData() {
	Log::file << "[Vehicle " << m_Vehicle << "] RemoveColorTextureData" << std::endl;

	std::vector<ColorTexture*> entries;

	for (auto pair : m_ColorTextureData) {
		entries.push_back(pair.first);
	}

	for (auto entry : entries) {
		delete m_ColorTextureData[entry];
	}

	m_ColorTextureData.clear();

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

std::map<CVehicle*, Vehicle*> Vehicles::m_Vehicles;