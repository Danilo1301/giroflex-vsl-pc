#include "VehicleDummy.h"

std::vector<RwFrame*> VehicleDummy::m_Frames;

void VehicleDummy::FindDummies(CVehicle* vehicle, RwFrame* frame, bool parent) {
	if (!frame)
		return;

	if (RwFrame* nextFrame = frame->child)
		FindDummies(vehicle, nextFrame, (RwFrameGetParent(frame)) ? (true) : (false));

	if (RwFrame* nextFrame = frame->next)
		FindDummies(vehicle, nextFrame, parent);

	if (std::find(m_Frames.begin(), m_Frames.end(), frame) != m_Frames.end())
		return;

	m_Frames.push_back(frame);
}

std::vector<RwFrame*> VehicleDummy::GetFramesOnVehicle(CVehicle* vehicle) {
	m_Frames.clear();
	FindDummies(vehicle, (RwFrame*)vehicle->m_pRwClump->object.parent);
	return m_Frames;
}