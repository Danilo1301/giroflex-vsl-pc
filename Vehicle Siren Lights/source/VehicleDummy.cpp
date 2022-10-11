#include "VehicleDummy.h"

std::vector<RwFrame*> VehicleDummy::m_Frames;
RwMatrix* tempMat = NULL;

void VehicleDummy::CreateTempMatrix()
{
	if (!tempMat) tempMat = RwMatrixCreate();
}

std::vector<RpAtomic*> VehicleDummy::RpClumpGetAllAtomics(RpClump* clump)
{
	static std::vector<RpAtomic*> atomics;
	atomics.clear();

	RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) {
		atomics.push_back(atomic);
		return atomic;
	}, (void*)((uint32_t)(0)));

	return atomics;
}

std::vector<RpMaterial*> VehicleDummy::RpGeometryGetAllMaterials(RpGeometry* geometry)
{
	static std::vector<RpMaterial*> materials;
	materials.clear();

	RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* data) {
		materials.push_back(material);
		return material;
	}, 0);

	return materials;
}

std::vector<RwFrame*> VehicleDummy::GetFrameHierarchy(RwFrame* frame, RwFrame* root) {
	std::vector<RwFrame*> hie;
	RwFrame* f = frame;

	while (f != NULL && f != root)
	{
		hie.insert(hie.begin(), f);
		f = RwFrameGetParent(f);
	}
	return hie;
}

/*
Takes the world position of a dummy and adds offset from its position
*/
CVector VehicleDummy::GetTransformedDummyPosition(CVehicle* vehicle, RwFrame* dummy, CVector offset) {
	CreateTempMatrix();

	auto rootFrame = (RwFrame*)vehicle->m_pRwClump->object.parent;

	RwMatrixTransform(tempMat, RwFrameGetMatrix(rootFrame), rwCOMBINEREPLACE);

	auto hierarchy = GetFrameHierarchy(dummy, rootFrame);
	for (auto hf : hierarchy) {
		RwMatrixTransform(tempMat, RwFrameGetMatrix(hf), rwCOMBINEPRECONCAT);
	}

	RwMatrixTranslate(tempMat, &offset.ToRwV3d(), rwCOMBINEPRECONCAT);

	return CVector(tempMat->pos.x, tempMat->pos.y, tempMat->pos.z);
}
