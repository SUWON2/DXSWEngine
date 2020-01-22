#pragma once

#include <memory>

#include "../Mesh/Mesh.h"
#include "../Material/Material.h"
#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class SkyDome final
{
public:
	SkyDome();

	SkyDome(const SkyDome&) = delete;

	SkyDome& operator=(const SkyDome&) = delete;

	~SkyDome();

public:
	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const DirectX::XMMATRIX& matViewProjection);

private:
	Mesh* mMesh = nullptr;

	Material* mMaterial = nullptr;
};