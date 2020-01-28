#include "SkyDome.h"
#include "../../../Common/Define.h"

using namespace DirectX;

SkyDome::SkyDome()
{
	mMaterial = Material::Create("Shaders/BasicSkyDomeShaderVS.hlsl", "Shaders/BasicSkyDomeShaderPS.hlsl");
	mMaterial->RegisterTexture(0, "Resource/BasicSky.dds");

	mModel = Model::Create("Resource/BasicSkyDome.model");
	mModel->SetMaterial(0, reinterpret_cast<size_t>(mMaterial));
}

SkyDome::~SkyDome()
{
	RELEASE(mMaterial);
	RELEASE(mModel);
}

void SkyDome::_Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const XMMATRIX& matViewProjection)
{
	mMaterial->_Activate({});
	mMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
	mMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection));

	mModel->_Draw({}, 0);
}
