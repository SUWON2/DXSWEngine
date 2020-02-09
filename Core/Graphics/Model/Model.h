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

	~Model() = default;

	static Model* Create(const char* fileName);

	const char* GetFileName() const;

	const std::unique_ptr<ID[]>& GetMaterialIds() const;
	
	size_t GetMeshCount() const;

	bool IsActive() const;

	const DirectX::XMFLOAT3& GetPosition() const;

	const DirectX::XMFLOAT3& GetScale() const;

	const DirectX::XMFLOAT3& GetRotation() const;

	void SetMaterial(const unsigned int materialIndex, const ID materialId);

	void SetActive(const bool bActive);

	void SetPosition(const DirectX::XMFLOAT3& position);

	void SetScale(const DirectX::XMFLOAT3& scale);

	void SetRotation(const DirectX::XMFLOAT3& rotation);

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Draw(RendererKey, const unsigned int meshIndex);

private:
	explicit Model(const char* fileName);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<ModelResource> mModelResource;

	std::unique_ptr<ID[]> mMaterialIds = nullptr;

	ID mDataId = 0;

	size_t mMeshCount = 0;

	bool mbActive = true;

	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 mRotation = {};
};