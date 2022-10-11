#include "Vehicle.h"

#include "VehicleDummy.h"
#include "LightGroups.h"
#include "TestHelper.h"
#include "Mod.h"
#include "Keybinds.h"
#include "menu/Menu.h"

//#include "windows/WindowLightGroup.h"

#include "CVisibilityPlugins.h"
#include "CShadows.h"

/*
struct Mat
{
	RwRGBA color;
	RwSurfaceProperties surfaceProps;
	RpMaterial* material;
};

auto val = reinterpret_cast<unsigned int>(material);

if (Materials.find(val) == Materials.end())
{
	Materials[val] = { material->color, material->surfaceProps, material };

	Log::file << val << ", " << name << std::endl;

*/

//static std::list<std::pair<unsigned int*, unsigned int>> ResetEntries;
//static std::map<unsigned int, Mat> Materials;


void CVector_AddDir(CVector* vec, CVector2D forward, CVector2D right, float x, float y)
{
	(*vec) += CVector(y * forward.x, y * forward.y, 0.0f);
	(*vec) += CVector(x * right.x, x * right.y, 0.0f);
}

Vehicle::Vehicle(CVehicle* veh)
{
	m_Vehicle = veh;	

	CheckForLightGroups();
}

void Vehicle::Update()
{
	CheckForLightGroups();
	UpdatePatternAndSteps();

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		if (lightGroup->keybindChange.CheckKeybind())
		{
			GoToNextLightGroupPattern(lightGroup);
		}

		if (lightGroup->keybindPause.CheckKeybind())
		{
			vehiclePatternData->paused = !vehiclePatternData->paused;
		}
	}

	RegisterCoronas();
}

void Vehicle::Draw()
{
	//
	char buffer[512];
	auto screenPos = WorldToScreen(m_Vehicle->GetPosition() + CVector(0, 0, -0.5f));

	sprintf(buffer, "Vehicle");
	DrawScreenText(buffer, screenPos);
	screenPos.y += 20.0f;

	//
	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		auto stepLoop = vehiclePatternData->stepLoop;
		auto patternLoop = vehiclePatternData->patternLoop;

		sprintf(buffer, "step: %d / %d steps, time=%d, total=%d", stepLoop->StepIndex, stepLoop->Steps.size(), stepLoop->Time, stepLoop->TotalTime);
		DrawScreenText(buffer, screenPos);
		screenPos.y += 20.0f;

		sprintf(buffer, "pattern: %d / %d steps, time=%d, total=%d", patternLoop->StepIndex, patternLoop->Steps.size(), patternLoop->Time, patternLoop->TotalTime);
		DrawScreenText(buffer, screenPos);
		screenPos.y += 20.0f;
	}

	//
	auto atomics = VehicleDummy::RpClumpGetAllAtomics(m_Vehicle->m_pRwClump);
	for (auto atomic : atomics)
	{
		RwFrame* frameAtomic = GetObjectParent((RwObject*)atomic);
		std::string name = GetFrameNodeName(frameAtomic);
		CVector position = VehicleDummy::GetTransformedDummyPosition(m_Vehicle, frameAtomic, CVector(0, 0, 0));

		int mats = -1;

		if (atomic->geometry)
		{
			auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
			mats = materials.size();
		}

		auto screenPos = WorldToScreen(position);
		sprintf(buffer, "%s | %d materials", name.c_str(), mats);
		//DrawScreenText(buffer, screenPos);
	}
}

void Vehicle::Destroy()
{

}

void Vehicle::CheckForLightGroups()
{
	if (!LightGroups::HasLightGroups(m_Vehicle->m_nModelIndex)) return;

	auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);

	for (auto lightGroup : lightGroups)
	{
		if (m_LightGroupData[lightGroup]) continue;

		/*
		if (lightGroup->patternCycleSteps.size() == 0) {
			m_LightGroupData.erase(lightGroup);
			continue;
		}
		*/

		TestHelper::AddLine("Add VehiclePatternData of " + lightGroup->name + " to vehicle " + std::to_string(reinterpret_cast<unsigned int>(m_Vehicle)) + " with " + std::to_string(lightGroup->patternCycleSteps.size()) + " patternCycleSteps");

		Log::file << "[Vehicle " << m_Vehicle << "] Add VehiclePatternData for light group '" << lightGroup->name << "' (" << std::to_string(m_LightGroupData.size()) << " total) with " << lightGroup->patternCycleSteps.size() << " patterns" << std::endl;

		auto vehiclePatternData = new VehiclePatternData();

		for (auto patternCycleStep : lightGroup->patternCycleSteps)
		{
			vehiclePatternData->patternLoop->AddStep(&patternCycleStep->duration);
		}

		//if (lightGroup->turnOnSiren) vehiclePatternData->enabled = GetSirenState();

		m_LightGroupData[lightGroup] = vehiclePatternData;
	}
}

void Vehicle::UpdatePatternAndSteps()
{
	auto dt = Mod::GetDeltaTime();

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		if (vehiclePatternData->paused || !vehiclePatternData->lightsOn) continue;

		auto stepLoop = vehiclePatternData->stepLoop;
		auto patternLoop = vehiclePatternData->patternLoop;

		patternLoop->Update(dt);

		if (patternLoop->HasStepChanged() || stepLoop->HasNoSteps())
		{
			//if (stepLoop->HasNoSteps()) TestHelper::AddLine("has no steps");
			//if (patternLoop->HasStepChanged()) TestHelper::AddLine("step changed");

			stepLoop->Clear();

			auto pattern = lightGroup->patternCycleSteps[patternLoop->StepIndex]->pattern;
			for (size_t i = 0; i < pattern->steps.size(); i++)
			{
				vehiclePatternData->stepLoop->AddStep(&pattern->steps[i]->duration);;
			}
		}

		stepLoop->Update(dt);
	}
}

void Vehicle::RegisterCoronas()
{
	//Log::file << "[Vehicle " << m_Vehicle << "] RegisterCoronas" << std::endl;

	
	float distanceFromPlayer = 0.0f;
	auto ped = FindPlayerPed(0);
	if (ped)
	{
		distanceFromPlayer = DistanceBetweenPoints(ped->GetPosition(), m_Vehicle->GetPosition());
	}

	int lightId = reinterpret_cast<unsigned int>(m_Vehicle) + 50;

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		auto patternLoop = vehiclePatternData->patternLoop;
		auto patternCycleStep = lightGroup->patternCycleSteps[patternLoop->StepIndex];
		auto patternDuration = patternCycleStep->duration;
		auto pattern = patternCycleStep->pattern;

		auto stepLoop = vehiclePatternData->stepLoop;
		auto step = pattern->steps[stepLoop->StepIndex];

		if (!step) continue; //maybe?

		lightId += lightGroup->offsetId;

		for (size_t point_i = 0; point_i < lightGroup->points.size(); point_i++)
		{
			auto point = lightGroup->points[point_i];
			CVector position = point->position;

			CRGBA color = point->color;
			bool enabled = true;
			
			if (lightGroup->isLightbar)
			{
				enabled = point->GetIsEnabled(step, point_i);
			}
			else {
				enabled = point->GetIsEnabled(step);
			}
			
			if (!vehiclePatternData->lightsOn) enabled = false;

			/*
			if (!m_FreezeLights) {
				if (!vehiclePatternData->enabled) enabled = false;
			}
			else { enabled = true; }
			*/

			//

			auto positionY = lightGroup->position.y + point->position.y;
			auto angle = Point::GetAngle(m_Vehicle, lightGroup->position + point->position);
			auto radiusMult = point->GetRadiusMultiplier(angle, lightGroup->direction, positionY);
			auto radius = lightGroup->size * radiusMult;

			auto flareType = lightGroup->flareType;
			if (!enabled || distanceFromPlayer > 70.0f) flareType = eCoronaFlareType::FLARETYPE_NONE;

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
				1000.0f,
				lightGroup->type,
				flareType,
				true, //enableReflection
				false, //checkObstacles
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

void Vehicle::GoToNextLightGroupPattern(LightGroup* lightGroup)
{
	auto vehiclePatternData = m_LightGroupData[lightGroup];

	vehiclePatternData->patternLoop->SetStep(vehiclePatternData->patternLoop->StepIndex + 1);
	vehiclePatternData->stepLoop->Clear();

	UpdatePatternAndSteps();
}

void Vehicle::RenderBefore()
{
	auto atomics = VehicleDummy::RpClumpGetAllAtomics(m_Vehicle->m_pRwClump);
	for (auto atomic : atomics)
	{
		if (!atomic->geometry) continue;

		atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

		RwFrame* frameAtomic = GetObjectParent((RwObject*)atomic);
		std::string name = GetFrameNodeName(frameAtomic);
		//CVector position = m_Vehicle->TransformFromObjectSpace(GetFrameNodePosition(frameAtomic));

		for (auto pair : m_LightGroupData)
		{
			auto lightGroup = pair.first;
			auto vehiclePatternData = pair.second;

			auto patternLoop = vehiclePatternData->patternLoop;
			auto patternCycleStep = lightGroup->patternCycleSteps[patternLoop->StepIndex];
			auto patternDuration = patternCycleStep->duration;
			auto pattern = patternCycleStep->pattern;

			auto stepLoop = vehiclePatternData->stepLoop;
			auto step = pattern->steps[stepLoop->StepIndex];

			if (!step) continue; //maybe?

			for (size_t point_i = 0; point_i < lightGroup->points.size(); point_i++)
			{
				auto point = lightGroup->points[point_i];

				
				//CVector position = point->position;

				CRGBA color = point->color;
				bool enabled = true;

				if (lightGroup->isLightbar)
				{
					//if (!starts_with(ToUpper(name), lightGroup->lightbarSettings.object_prefix)) continue;
					if (ToUpper(name).compare(ToUpper(lightGroup->lightbarSettings.object_prefix + std::to_string(point_i+1))) != 0) continue;

					enabled = point->GetIsEnabled(step, point_i);
				}
				else {
					if (ToUpper(name).compare(ToUpper(point->object)) != 0) continue;

					enabled = point->GetIsEnabled(step);
				}

				TestHelper::AddLine("found " + name);

				if (!vehiclePatternData->lightsOn) enabled = false;

				auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
				for (auto material : materials)
				{
					//if (!material) continue;

					if (enabled) {
						material->color = { color.r, color.g, color.b, color.a };
					} else {
						material->color = { point->disabledColor.r, point->disabledColor.g, point->disabledColor.b, point->disabledColor.a };
					}

					material->surfaceProps.ambient = 10;
					material->surfaceProps.diffuse = 10;
					material->surfaceProps.specular = 10;
				}
			}
		}
	}


	//char buffer[512];


	

	/*
	auto atomics = VehicleDummy::RpClumpGetAllAtomics(m_Vehicle->m_pRwClump);
	for (auto atomic : atomics)
	{
		RwFrame* frameAtomic = GetObjectParent((RwObject*)atomic);
		std::string name = GetFrameNodeName(frameAtomic);

		if (name.find("giroflex_led_2") == std::string::npos) continue;

		if (atomic->geometry)
		{
			auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
			for (auto material : materials)
			{
				if (!material) continue;

				if (Input::GetKey(65))
				{
					//ResetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&material->color), *reinterpret_cast<unsigned int*>(&material->color)));

					
					material->color.red = 0;
					material->color.green = 255;
					material->color.blue = 0;
					material->color.alpha = 255;
					
				}

				

				//if (!material || !material->texture) continue;
			}
		}
	}
	*/
}

void Vehicle::RenderAfter()
{
	/*
	for (auto& p : ResetEntries)
	{
		if (std::find(DontResetMaterials.begin(), DontResetMaterials.end(), p.first) != DontResetMaterials.end())
			continue;

		*p.first = p.second;
	}
	ResetEntries.clear();
	*/
}