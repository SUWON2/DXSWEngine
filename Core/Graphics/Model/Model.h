#pragma once

#include <memory>

#include "ModelResource.h"
#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class Model final
{
public:
	Model(const Model&) = delete;

	Model& operator=(const Model&) = delete;

	~Model();

	static Model* Create(const char* fileName);

	inline const char* GetFileName() const
	{
		return mModelResource->GetResourceName(mDataId).c_str();
	}

	inline const std::unique_ptr<size_t[]>& GetMaterialIds() const
	{
		return mMaterialIds;
	}

	inline unsigned int GetMeshCount() const
	{
		return mMeshCount;
	}

	inline bool IsActive() const
	{
		return mbActive;
	}

	inline const DirectX::XMFLOAT3& GetPosition() const
	{
		return mPosition;
	}

	inline const DirectX::XMFLOAT3& GetScale() const
	{
		return mScale;
	}

	inline const DirectX::XMFLOAT3& GetRotation() const
	{
		return mRotation;
	}

	void SetMaterial(const unsigned int materialIndex, const size_t materialId);

	inline void SetActive(const bool bActive)
	{
		mbActive = bActive;
	}

	inline void SetPosition(const DirectX::XMFLOAT3& position)
	{
		mPosition = position;
	}

	inline void SetScale(const DirectX::XMFLOAT3& scale)
	{
		mScale = scale;
	}

	inline void SetRotation(const DirectX::XMFLOAT3& rotation)
	{
		mRotation = rotation;
	}

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Draw(RendererKey, const unsigned int meshIndex);

private:
	explicit Model(const char* fileName);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<ModelResource> mModelResource;

	std::unique_ptr<size_t[]> mMaterialIds = nullptr;

	size_t mDataId = 0;

	unsigned int mMeshCount = 0;

	bool mbActive = true;

	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 mRotation = {};
};