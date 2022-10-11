#pragma once

#include "pch.h"

class VehicleDummy {
private:
	static std::vector<RwFrame*> m_Frames;
	static void CreateTempMatrix();
public:
	
	static std::vector<RpAtomic*> RpClumpGetAllAtomics(RpClump* clump);
	static std::vector<RpMaterial*> RpGeometryGetAllMaterials(RpGeometry* geometry);

	static std::vector<RwFrame*> GetFrameHierarchy(RwFrame* frame, RwFrame* root);
	static CVector GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset);
};