#include "SkyDome.h"
#include "../../../Common/Define.h"

using namespace DirectX;

SkyDome::SkyDome()
{
	mMaterial = new Material({}, "Shaders/BasicSkyDomeVS.hlsl", "Shaders/BasicSkyDomePS.hlsl");
	mMaterial->RegisterTexture(0, "Resource/BasicSky.dds");

	Model* skyDome;
	mModelFrame = new ModelFrame({}, "Resource/BasicSkyDome.model", { mMaterial });
	mModelFrame->Create(&skyDome);
}

SkyDome::~SkyDome()
{
	RELEASE(mMaterial);
	RELEASE(mModelFrame);
}

bool SkyDome::IsActive() const
{
	return mbActive;
}

void SkyDome::SetActive(const bool bActive)
{
	mbActive = bActive;
}

void SkyDome::_Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const XMMATRIX& matViewProjection)
{
	mMaterial->_Active({}, nullptr, matViewProjection);
	mMaterial->UpdateBuffer<Material::ShaderType::VS>(0, XMMatrixTranspose(matWorld));

	mModelFrame->_DrawMesh({}, 0);
}
