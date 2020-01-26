#pragma once

#include <memory>
#include <string>

#include "MeshResource.h"
#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class Mesh final
{
public:
	Mesh(const Mesh&) = delete;

	Mesh& operator=(const Mesh&) = delete;

	~Mesh();

	static Mesh* Create(const char* fileName, const size_t materialID);

	inline const char* GetFileName() const
	{
		return mMeshResource->GetResourceName(mVertexBufferID).c_str();
	}

	inline const size_t GetMaterialID() const
	{
		return mMaterialID;
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

	inline bool IsActive() const
	{
		return mbActive;
	}

	inline void SetMaterialID(const size_t materialID)
	{
		mMaterialID = materialID;
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

	inline void SetActive(const bool bActive)
	{
		mbActive = bActive;
	}

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Draw(RendererKey);

private:
	explicit Mesh(const char* fileName, const size_t materialID);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<MeshResource> mMeshResource;

	int mVertexBufferID = 0;

	size_t mMaterialID = 0;

	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 mRotation = {};

	bool mbActive = true;
};