#include "LightGroupShadow.h"

#include "CTxdStore.h"

std::vector<RwTexture*> LightGroupShadows::m_ShadowTextures;

void LightGroupShadows::LoadTextures()
{
	int txd = CTxdStore::AddTxdSlot("vsl_shadows");
	CTxdStore::LoadTxd(txd, PLUGIN_PATH("shadows.txd"));
	CTxdStore::AddRef(txd);
	CTxdStore::PushCurrentTxd();
	CTxdStore::SetCurrentTxd(txd);

	for (int i = 0; i < NUM_OF_SHADOWS; i++)
	{
		Log::file << "Loading shadow texture " << i << " from shadows.txd" << std::endl;

		char str[256];
		sprintf(str, "%d", i);

		CSprite2d shadowSprite;
		shadowSprite.SetTexture(str, str);

		auto rwTexture = shadowSprite.m_pTexture;

		Log::file << "Loaded RwTexture: " << rwTexture << std::endl;

		m_ShadowTextures.push_back(rwTexture);
	}

	CTxdStore::PopCurrentTxd();
}