#pragma once

#include <memory>

#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class Model final
{
public:
	Model(int materialCount);

	Model(const Model&) = delete;

	Model& operator=(const Model&) = delete;

	~Model() = default;

	bool IsActive() const;

	const DirectX::XMFLOAT3& GetPosition() const;

	const DirectX::XMFLOAT3& GetScale() const;

	const DirectX::XMFLOAT3& GetRotation() const;

	int GetTextureIndex(const int materialIndex);

	void SetActive(const bool bActive);

	void SetPosition(const DirectX::XMFLOAT3& position);

	void SetScale(const DirectX::XMFLOAT3& scale);

	void SetRotation(const DirectX::XMFLOAT3& rotation);

	void SetTextureIndex(const int materialIndex, const int textureIndex);

private:
	bool mbActive = true;

	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mScale = { 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 mRotation = {};

	std::unique_ptr<int[]> mTextureIndices = nullptr;

	#if defined(DEBUG) | defined(_DEBUG)
	int mMaterialCount;
	#endif
};