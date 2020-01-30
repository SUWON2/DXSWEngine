#pragma once

#include <d3d11.h>
#include <memory>

#include "TextResource.h"
#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class Text final
{
	struct Vertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoord;
	};

public:
	enum class VerticalAnchor
	{
		Top,
		Middle,
		Bottom,
	};

	enum class HorizontalAnchor
	{
		Left,
		Center,
		Right
	};

public:
	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	~Text();

	// 폰트 데이터와 머티리얼을 따로 지정하지 않아도 기본적으로 제공하는 폰트가 적용됩니다.
	static inline Text* Create()
	{
		return new Text;
	}

	static inline Text* Create(const char* fontDataName, const size_t materialId)
	{
		return new Text(fontDataName, materialId);
	}

	inline size_t GetMaterialId() const
	{
		return mMaterialId;
	}

	inline bool IsActive() const
	{
		return mbActive;
	}

	inline VerticalAnchor GetVerticalAnchor() const
	{
		return mVerticalAnchor;
	}

	inline HorizontalAnchor GetHorizontalAnchor() const
	{
		return mHorizontalAnchor;
	}

	inline const DirectX::XMFLOAT2& GetAnchorPoint() const
	{
		return mAnchorPoint;
	}

	inline const DirectX::XMFLOAT2& GetPosition() const
	{
		return mPosition;
	}

	void SetSentence(const char* sentence);

	inline void SetMaterialId(const size_t materialId)
	{
		mMaterialId = materialId;
	}

	inline void SetActive(const bool bActive)
	{
		mbActive = bActive;
	}

	void SetVerticalAnchor(const VerticalAnchor verticalAnchor);

	void SetHorizontalAnchor(const HorizontalAnchor horizontalAnchor);

	inline void SetPosition(const DirectX::XMFLOAT2& position)
	{
		mPosition = position;
	}

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Draw(RendererKey);

private:
	Text();

	Text(const char* fontDataName, const size_t materialId);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<TextResource> mTextResource;

	ID3D11Buffer* mSentenceVertexBuffer = nullptr;

	size_t mVertexCount = 0;

	size_t mMaxSentenceLength = 0;

	size_t mFontDataId = 0;

	size_t mMaterialId = 0;

	bool mbActive = true;

	VerticalAnchor mVerticalAnchor = VerticalAnchor::Middle;

	HorizontalAnchor mHorizontalAnchor = HorizontalAnchor::Center;

	DirectX::XMFLOAT2 mAnchorPoint = {};

	DirectX::XMFLOAT2 mPosition = {};
};