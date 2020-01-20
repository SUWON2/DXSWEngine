#pragma once

#include <d3d11.h>
#include <memory>

#include "TextResource.h"
#include "../RenederKey.h"
#include "../../../Common/DirectXMath.h"

class Text final
{
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

	static inline Text* Create()
	{
		return new Text();
	}

	static inline Text* Create(const char* fontDataName, const size_t materialID)
	{
		return new Text(fontDataName, materialID);
	}

	void SetSentence(const char* sentence);

	inline size_t GetMaterialID() const
	{
		return mMaterialID;
	}

	inline const DirectX::XMFLOAT2& GetPosition() const
	{
		return mPosition;
	}

	inline void SetMaterialID(const size_t materialID)
	{
		mMaterialID = materialID;
	}

	inline void SetPosition(const DirectX::XMFLOAT2& position)
	{
		mPosition = position;
	}

public:
	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	static void Initialize(RenederKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void Draw(RenederKey);

private:
	// ��Ʈ �����Ϳ� ��Ƽ������ ���� �������� �ʾƵ� �⺻������ �����ϴ� ��Ʈ�� ����˴ϴ�.
	Text();

	Text(const char* fontDataName, const size_t materialID);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<TextResource> mTextResource;

	ID3D11Buffer* mSentenceVertexBuffer = nullptr;

	size_t mVertexCount = 0;

	size_t mMaxSentenceLength = 0;

	size_t mFontDataID = 0;

	size_t mMaterialID = 0;

	DirectX::XMFLOAT2 mPosition = {};
};