#pragma once

#include <memory>

#include "../RendererKey.h"
#include "../Model/ModelFrame.h"
#include "../Material/Material.h"
#include "../../../Common/DirectXMath.h"

class SkyDome final
{
public:
	SkyDome();

	SkyDome(const SkyDome&) = delete;

	SkyDome& operator=(const SkyDome&) = delete;

	~SkyDome();

	bool IsActive() const;

	void SetActive(const bool bActive);

public:
	void _Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const DirectX::XMMATRIX& matViewProjection);

private:
	ModelFrame* mModelFrame = nullptr;

	Material* mMaterial = nullptr;

	bool mbActive = true;
};