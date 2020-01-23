#include "SkyDome.h"
#include "../../../Common/Define.h"

using namespace DirectX;

SkyDome::SkyDome()
{
	mMaterial = Material::Create("Shaders/BasicSkyDomeShaderVS.hlsl", "Shaders/BasicSkyDomeShaderPS.hlsl");
	mMaterial->RegisterTexture(0, "Resource/BasicSky.dds");

	mMesh = Mesh::Create("Resource/BasicSkyDome.mesh", reinterpret_cast<size_t>(&mMaterial));
}

SkyDome::~SkyDome()
{
	RELEASE(mMaterial);
	RELEASE(mMesh);
}

void SkyDome::Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const XMMATRIX& matViewProjection)
{
	mMaterial->Activate({});
	mMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
	mMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection));

	mMesh->Draw({});
}
