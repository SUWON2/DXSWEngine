#pragma once

#include <memory>

#include "../Model/Model.h"
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

	inline bool IsActive() const
	{
		return mbActive;
	}

	inline void SetActive(const bool bActive)
	{
		mbActive = bActive;
	}

public:
	void _Draw(RendererKey, const DirectX::XMMATRIX& matWorld, const DirectX::XMMATRIX& matViewProjection);

private:
	Model* mModel = nullptr;

	Material* mMaterial = nullptr;

	bool mbActive = true;
};