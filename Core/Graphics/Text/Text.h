#pragma once

#include <d3d11.h>
#include <memory>

#include "TextResource.h"
#include "../RendererKey.h"
#include "../../../Common/DirectXMath.h"

class Text final
{
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

private:
	struct Vertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoord;
	};

public:
	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	~Text();

	// 폰트 데이터와 머티리얼을 따로 지정하지 않아도 기본적으로 제공하는 폰트가 적용됩니다.
	static Text* Create();

	static Text* Create(const char* fontDataName, const ID materialId);

	ID GetMaterialId() const;

	bool IsActive() const;

	VerticalAnchor GetVerticalAnchor() const;

	HorizontalAnchor GetHorizontalAnchor() const;

	const DirectX::XMFLOAT2& GetAnchorPoint() const;

	const DirectX::XMFLOAT2& GetPosition() const;

	void SetSentence(const char* sentence);

	void SetMaterialId(const ID materialId);

	void SetActive(const bool bActive);

	void SetVerticalAnchor(const VerticalAnchor verticalAnchor);

	void SetHorizontalAnchor(const HorizontalAnchor horizontalAnchor);

	void SetPosition(const DirectX::XMFLOAT2& position);

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Draw(RendererKey);

private:
	Text();

	Text(const char* fontDataName, const ID materialId);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<TextResource> mTextResource;

	ID3D11Buffer* mSentenceVertexBuffer = nullptr;

	UINT mVertexCount = 0;

	UINT mMaxSentenceLength = 0;

	ID mFontDataId = 0;

	ID mMaterialId = 0;

	bool mbActive = true;

	VerticalAnchor mVerticalAnchor = VerticalAnchor::Middle;

	HorizontalAnchor mHorizontalAnchor = HorizontalAnchor::Center;

	DirectX::XMFLOAT2 mAnchorPoint = {};

	DirectX::XMFLOAT2 mPosition = {};
};