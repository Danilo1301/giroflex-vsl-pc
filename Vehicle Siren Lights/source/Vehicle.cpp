#include "Vehicle.h"

#include "VehicleDummy.h"
#include "Mod.h"
#include "LightGroups.h"
#include "log/Log.h"

#include "menu/Menu.h"
#include "windows/WindowLightGroup.h"
#include "windows/WindowLightgroupMenu.h"

#include "CVisibilityPlugins.h"
#include "CShadows.h"

float Vehicle::m_MatAmbient = 3.5f;
bool Vehicle::m_FreezeLights = false;

static std::list<std::pair<unsigned int*, unsigned int>> m_ResetEntries;

void CVector_AddDir(CVector* vec, CVector2D forward, CVector2D right, float x, float y)
{
	(*vec) += CVector(y * forward.x, y * forward.y, 0.0f);
	(*vec) += CVector(x * right.x, x * right.y, 0.0f);
}

Vehicle::Vehicle(CVehicle* veh) {
	m_Vehicle = veh;	

	CheckForLightGroups();
}

void Vehicle::Update() {
	CheckForLightGroups();

	if (!m_FreezeLights)
	{
		m_RotateShadowAngle += Mod::GetDeltaTime() * 0.5f;
		while (m_RotateShadowAngle >= 360) m_RotateShadowAngle -= 360;
	}

	UpdatePatternAndSteps();

	/*
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
	*/

	UpdateSirenState();
	RegisterCoronas();
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
	


	/*
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
	/DrawScreenText(buffer, screenPos);
	}
	*/

	/*
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
	*/

	/*
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
	*/

	/*
	sprintf(text, "[time] %d %d", CTimer::m_snTimeInMilliseconds, m_PrevTime);
	DrawScreenText(text, screenPos);
	screenPos.y += 20.0f;
	*/
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

			auto amountOfPoints = lightGroup->points.size();
			auto distance = lightGroup->distance;
			auto curve = lightGroup->curve;

			//position
			float x = (i * distance) - ((amountOfPoints - 1) * distance / 2);
			float y = (float)arch_fn_parabola((float)(i), curve, (float)(amountOfPoints - 1));
			auto position = VehicleDummy::GetTransformedPosition(m_Vehicle, CVector(x, y, 0) + lightGroup->position + point->position);

			//fixed position
			if (lightGroup->pointsPositionFixed)
				position = VehicleDummy::GetTransformedPosition(m_Vehicle, lightGroup->position + point->position);

			CFont::SetOrientation(ALIGN_CENTER);
			CFont::SetColor(CRGBA(255, 255, 255));
			CFont::SetDropShadowPosition(1);
			CFont::SetBackground(false, false);
			CFont::SetWrapx(500.0);
			CFont::SetScale(0.5 * 0.5f, 1.0 * 0.5f);
			CFont::SetFontStyle(FONT_SUBTITLES);
			CFont::SetProportional(true);

			RwV3d rwp = { position.x, position.y, position.z };
			RwV3d screenCoors; float w, h;

			CSprite::CalcScreenCoors(rwp, &screenCoors, &w, &h, true, true);

			sprintf(text, "%d", i + 1);
			CFont::PrintString(screenCoors.x, screenCoors.y, text);

			i++;
		}
	}
}

void Vehicle::Destroy() {

}

void Vehicle::ResetObjectRotation(std::string object)
{
	auto frame = VehicleDummy::FindDummy(m_Vehicle, object);

	if (!frame) return;

	RwV3d axis = {0, 0, 0};
	RwReal angle = 0.0f;
	RwV3d pos = frame->modelling.pos;
	
	RwMatrixRotate(&frame->modelling, &axis, angle, rwCOMBINEREPLACE);
	RwMatrixTranslate(&frame->modelling, &pos, rwCOMBINEREPLACE);
}

void Vehicle::GoToNextLightGroupPattern(LightGroup* lightGroup)
{
	auto vehiclePatternData = m_LightGroupData[lightGroup];

	vehiclePatternData->patternLoop->SetStep(vehiclePatternData->patternLoop->StepIndex + 1);
	vehiclePatternData->stepLoop->Clear();

	UpdatePatternAndSteps();
}

bool Vehicle::GetSirenState() {
	if ((Menu::m_IsOpen && !WindowLightgroupMenu::m_Window) && FindPlayerVehicle(0, false) == m_Vehicle) return true;
	return m_Vehicle->m_nVehicleFlags.bSirenOrAlarm;
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

			if (!step) continue;

			for (size_t point_i = 0; point_i < lightGroup->points.size(); point_i++)
			{
				auto point = lightGroup->points[point_i];
				auto amountOfPoints = lightGroup->points.size();

				//color
				CRGBA color = lightGroup->color1;
				if ((double)point_i < ((double)amountOfPoints) / (double)2)
				{
					color = lightGroup->color1;

					if (amountOfPoints % 2 == 1 && point_i == std::round(amountOfPoints / 2) && amountOfPoints > 2)
					{
						color = lightGroup->color3;
					}
				}
				else {
					color = lightGroup->color2;
				}

				//custom color
				if (point->useCustomColor)
					color = point->customColor;

				//custom enabled color
				if (point->useCustomEnabledColor)
					color = point->enabledColor;

				//led colors
				if (lightGroup->lightbarSettings.useLightbarLeds)
				{
					color = lightGroup->lightbarSettings.ledOnColor;
				}

				bool enabled = true;

				if (name.length() == 0) continue;

				if (lightGroup->lightbarSettings.useLightbarLeds)
				{
					if (ToUpper(name).compare(ToUpper(lightGroup->lightbarSettings.object_prefix + std::to_string(point_i + 1))) != 0) continue;
				}
				else {
					if (ToUpper(name).compare(ToUpper(point->object)) != 0) continue;
				}

				enabled = point->GetIsEnabled(step, point_i);

				//TestHelper::AddLine("found " + name);

				if (!vehiclePatternData->lightsOn) enabled = false;
				if (m_FreezeLights) enabled = true;

				if (!enabled)
				{
					color = point->disabledColor;

					if (lightGroup->lightbarSettings.useLightbarLeds)
						color = lightGroup->lightbarSettings.ledOffColor;
				}

				//

				if (point->rotateObject.rotate)
				{
					if (vehiclePatternData->lightsOn || point->rotateObject.rotateAlways)
					{
						auto axisVal = point->rotateObject.axis;

						RwV3d axis = {
							(float)(axisVal == eRotateObjectAxis::X ? 1 : 0),
							(float)(axisVal == eRotateObjectAxis::Y ? 1 : 0),
							(float)(axisVal == eRotateObjectAxis::Z ? 1 : 0)
						};
						RwReal angle = point->rotateObject.speed;

						RwMatrixRotate(&frameAtomic->modelling, &axis, angle, rwCOMBINEPRECONCAT);
					}
				}

				//

				auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
				for (auto material : materials)
				{
					//if (!material) continue;

					material->color = { color.r, color.g, color.b, color.a };
					

					material->surfaceProps.ambient = 10;
					material->surfaceProps.diffuse = 10;
					material->surfaceProps.specular = 10;

					/*
					material->surfaceProps.ambient = m_MatAmbient;
					material->surfaceProps.diffuse = m_MatAmbient;
					material->surfaceProps.specular = m_MatAmbient;
					*/
				}
			}
		}
	}
}

void Vehicle::RenderAfter() {
	for (auto& p : m_ResetEntries) *p.first = p.second;
	m_ResetEntries.clear();

	RenderShadows();
}

void Vehicle::RenderShadows()
{
	int shadowId = (uintptr_t)m_Vehicle + 100;

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

		if (!step) continue;

		for (size_t point_i = 0; point_i < lightGroup->points.size(); point_i++)
		{
			auto point = lightGroup->points[point_i];
			auto amountOfPoints = lightGroup->points.size();
			auto distance = lightGroup->distance;
			auto curve = lightGroup->curve;

			//position
			float x = (point_i * distance) - ((amountOfPoints - 1) * distance / 2);
			float y = (float)arch_fn_parabola((float)(point_i), curve, (float)(amountOfPoints - 1));
			//auto position = CVector(x, y, 0) + lightGroup->position + point->position;

			if(!point->shadow.enabled) continue;

			auto shadow = point->shadow;

			//auto pos = VehicleDummy::GetTransformedPosition(m_Vehicle, lightGroup->position + point->position + CVector(point->shadow.position.x, point->shadow.position.y, 0));
			CVector pos = m_Vehicle->GetPosition();

			//

			CVector2D forward = CVector2D(m_Vehicle->m_matrix->up);
			CVector2D_Normalize(&forward);

			CVector2D right = CVector2D(m_Vehicle->m_matrix->right);
			CVector2D_Normalize(&right);

			auto offsetPos = lightGroup->position + CVector(x, y, 0) + point->position + CVector(shadow.position.x, shadow.position.y, 0);

			//fixed position
			if (lightGroup->pointsPositionFixed)
				offsetPos = lightGroup->position + point->position + CVector(shadow.position.x, shadow.position.y, 0);

			CVector_AddDir(&pos, forward, right, offsetPos.x, offsetPos.y);

			//

			float width = shadow.width;
			float height = shadow.height;

			if (shadow.rotate)
			{
				auto angle = m_RotateShadowAngle + shadow.rotateOffsetAngle;

				while (angle >= 360)
					angle -= 360;

				CVector2D_Rotate(&forward, angle);
				CVector2D_Rotate(&right, angle);

				CVector_AddDir(&pos, forward, right, 0, shadow.rotateOffsetPos);
			}
			else {
				CVector2D_Rotate(&forward, shadow.angle);
				CVector2D_Rotate(&right, shadow.angle);
			}

			//

			//color
			CRGBA color = lightGroup->color1;
			if ((double)point_i < ((double)amountOfPoints) / (double)2)
			{
				color = lightGroup->color1;

				if (amountOfPoints % 2 == 1 && point_i == std::round(amountOfPoints / 2) && amountOfPoints > 2)
				{
					color = lightGroup->color3;
				}
			}
			else {
				color = lightGroup->color2;
			}

			//custom color
			if (point->useCustomColor)
			{
				color = point->customColor;
			}

			bool enabled = true;
			
			enabled = point->GetIsEnabled(step, point_i);
			
			if (!vehiclePatternData->lightsOn) enabled = false;
			if (m_FreezeLights) enabled = true;

			if (!enabled) continue;

			float intensity = shadow.intensity;

			CShadows::StoreCarLightShadow(
				m_Vehicle,
				shadowId++,
				LightGroupShadows::m_ShadowTextures[shadow.textureIndex], // gpShadowExplosionTex, //LightGroupShadows::m_ShadowTexture,
				&pos,
				height * forward.x,
				height * forward.y,
				width * forward.y,
				-width * forward.x,
				ucharIntensity(color.r, intensity),
				ucharIntensity(color.g, intensity),
				ucharIntensity(color.b, intensity),
				800.0f
			);
		}
	}
}

void Vehicle::UpdateVehicleMaterial(RpMaterial* material, std::string frameName) {

}

void Vehicle::SetAllLightGroupState(bool enabled)
{
	//Log::file << "[Vehicle " << m_Vehicle << "] SetAllLightGroupState" << std::endl;

	m_PrevLightState = enabled;

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		vehiclePatternData->patternLoop->Reset();
		vehiclePatternData->stepLoop->Clear();

		if (lightGroup->turnOnSiren) {
			vehiclePatternData->lightsOn = enabled;
		}
	}
}

void Vehicle::CheckForLightGroups() {
	if (!LightGroups::HasLightGroups(m_Vehicle->m_nModelIndex)) return;

	auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);

	for (auto lightGroup : lightGroups)
	{
		if (m_LightGroupData[lightGroup]) continue;

		if (lightGroup->patternCycleSteps.size() == 0) {
			m_LightGroupData.erase(lightGroup);
			continue;
		}

		//TestHelper::AddLine("Add VehiclePatternData of " + lightGroup->name + " to vehicle " + std::to_string(reinterpret_cast<unsigned int>(m_Vehicle)) + " with " + std::to_string(lightGroup->patternCycleSteps.size()) + " patternCycleSteps");

		Log::file << "[Vehicle " << m_Vehicle << "] Add VehiclePatternData for light group '" << lightGroup->name << "' (" << std::to_string(m_LightGroupData.size()) << " total) with " << lightGroup->patternCycleSteps.size() << " patterns" << std::endl;

		auto vehiclePatternData = new VehiclePatternData();

		for (auto patternCycleStep : lightGroup->patternCycleSteps)
		{
			vehiclePatternData->patternLoop->AddStep(&patternCycleStep->duration);
		}

		if (lightGroup->turnOnSiren) vehiclePatternData->lightsOn = GetSirenState();

		m_LightGroupData[lightGroup] = vehiclePatternData;
	}
}

void Vehicle::UpdatePatternAndSteps() {

	//Log::file << "[Vehicle " << m_Vehicle << "] UpdatePatternAndSteps " << m_LightGroupData.size() << " light groups" << std::endl;

	auto dt = Mod::GetDeltaTime();

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto vehiclePatternData = pair.second;

		vehiclePatternData->patternLoop->DontChangeSteps = !vehiclePatternData->autoChangePattern; //manually change

		if (vehiclePatternData->paused || !vehiclePatternData->lightsOn) continue;

		auto stepLoop = vehiclePatternData->stepLoop;
		auto patternLoop = vehiclePatternData->patternLoop;

		patternLoop->Update(dt);

		if (patternLoop->HasStepChanged() || stepLoop->HasNoSteps())
		{
			TestHelper::AddLine("pattern step changed or step loop has no steps");

			//pattern offset
			if (stepLoop->HasNoSteps())
			{
				TestHelper::AddLine("pattern offset applied");

				int patternOffset = lightGroup->patternOffset;
				while (patternOffset > 0)
				{
					patternLoop->StepIndex++;
					if (patternLoop->StepIndex >= (int)patternLoop->Steps.size()) patternLoop->StepIndex = 0;
					patternOffset--;
				}
			}

			//

			stepLoop->Clear();

			auto pattern = lightGroup->patternCycleSteps[patternLoop->StepIndex]->pattern;

			for (size_t i = 0; i < pattern->steps.size(); i++)
			{
				stepLoop->AddStep(&pattern->steps[i]->duration);
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

		if (!step) continue;

		lightId += lightGroup->offsetId;

		for (size_t point_i = 0; point_i < lightGroup->points.size(); point_i++)
		{
			auto point = lightGroup->points[point_i];

			auto amountOfPoints = lightGroup->points.size();
			auto distance = lightGroup->distance;
			auto curve = lightGroup->curve;

			//position
			float x = (point_i * distance) - ((amountOfPoints - 1) * distance / 2);
			float y = (float)arch_fn_parabola((float)(point_i), curve, (float)(amountOfPoints - 1));
			auto position = CVector(x, y, 0) + lightGroup->position + point->position;

			//fixed position
			if (lightGroup->pointsPositionFixed)
				position = lightGroup->position + point->position;

			//color
			CRGBA color = lightGroup->color1;
			if ((double)point_i < ((double)amountOfPoints) / (double)2)
			{
				color = lightGroup->color1;

				if (amountOfPoints % 2 == 1 && point_i == std::round(amountOfPoints / 2) && amountOfPoints > 2)
				{
					color = lightGroup->color3;
				}
			}
			else {
				color = lightGroup->color2;
			}

			//custom color
			if (point->useCustomColor)
			{
				color = point->customColor;
			}

			bool enabled = true;

			enabled = point->GetIsEnabled(step, point_i);

			if (!vehiclePatternData->lightsOn) enabled = false;
			if (m_FreezeLights) enabled = true;

			auto positionY = lightGroup->position.y + point->position.y;
			auto angle = Point::GetAngle(m_Vehicle, lightGroup->position + point->position);
			auto radiusMult = point->GetRadiusMultiplier(angle, lightGroup->direction, positionY);
			auto radius = lightGroup->size * radiusMult;

			//

			auto flareType = lightGroup->flareType;
			//if (!enabled || distanceFromPlayer > 70.0f) flareType = eCoronaFlareType::FLARETYPE_NONE;
			if (flareType != eCoronaFlareType::FLARETYPE_NONE)
			{
				float distScale = 1.0f - (distanceFromPlayer / lightGroup->flareDistance);
				distScale *= lightGroup->flareIntensity;

				unsigned char flareAlpha = ucharIntensity(color.a, distScale);

				if (!enabled) flareAlpha = 0;

				CCoronas::RegisterCorona(
					lightId++,
					m_Vehicle,
					color.r,
					color.g,
					color.b,
					flareAlpha,
					position,
					0.0f,
					1000.0f,
					lightGroup->type,
					flareType,
					false, //enableReflection
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
			}

			//

			if (lightGroup->useSmallWhiteCorona)
			{
				CCoronas::RegisterCorona(
					lightId++,
					m_Vehicle,
					255,
					255,
					255,
					color.a,
					position,
					enabled ? (radius * lightGroup->smallWhiteCoronaScale) : 0.0f,
					1000.0f,
					lightGroup->smallWhiteCoronaType,
					eCoronaFlareType::FLARETYPE_NONE,
					false, //enableReflection
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
			}

			CCoronas::RegisterCorona(
				lightId++,
				m_Vehicle,
				color.r,
				color.g,
				color.b,
				color.a,
				position,
				enabled ? radius : 0.0f,
				1000.0f,
				lightGroup->type,
				eCoronaFlareType::FLARETYPE_NONE,
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
				CRGBA reflectColor = CRGBA(
					ucharIntensity(color.r, lightGroup->reflectionIntensity),
					ucharIntensity(color.g, lightGroup->reflectionIntensity),
					ucharIntensity(color.b, lightGroup->reflectionIntensity),
					color.a
				);

				CVector fposition = m_Vehicle->TransformFromObjectSpace(position);
				Command< 0x09E5 >(fposition.x, fposition.y, fposition.z, (int)reflectColor.r, (int)reflectColor.g, (int)reflectColor.b, lightGroup->reflectionDistance);
			}
		}
	}
	
}

void Vehicle::RemoveLightGroupDatas()
{
	std::vector<LightGroup*> toDelete;
	for (auto pair : m_LightGroupData) toDelete.push_back(pair.first);

	for (auto lightGroup : toDelete)
	{
		auto vehiclePatternData = m_LightGroupData[lightGroup];
		m_LightGroupData.erase(lightGroup);
		delete vehiclePatternData;
	}
}