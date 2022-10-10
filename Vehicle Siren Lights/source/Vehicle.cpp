#include "Vehicle.h"
#include "VehicleDummy.h"
#include "menu/Menu.h"

//#include "windows/WindowLightGroup.h"

#include "CVisibilityPlugins.h"
#include "CShadows.h"

static std::list<std::pair<unsigned int*, unsigned int>> ResetEntries;

void CVector_AddDir(CVector* vec, CVector2D forward, CVector2D right, float x, float y)
{
	(*vec) += CVector(y * forward.x, y * forward.y, 0.0f);
	(*vec) += CVector(x * right.x, x * right.y, 0.0f);
}

Vehicle::Vehicle(CVehicle* veh)
{
	m_Vehicle = veh;	
}

void Vehicle::Update()
{
	RegisterCoronas();
}

void Vehicle::Draw()
{
}

void Vehicle::Destroy()
{

}

void Vehicle::RenderBefore()
{
	
}

void Vehicle::RenderAfter()
{
	for (auto& p : ResetEntries) *p.first = p.second;
	ResetEntries.clear();
}

void Vehicle::RegisterCoronas()
{
	//Log::file << "[Vehicle " << m_Vehicle << "] RegisterCoronas" << std::endl;

	int lightId = reinterpret_cast<unsigned int>(m_Vehicle) + 50;
}