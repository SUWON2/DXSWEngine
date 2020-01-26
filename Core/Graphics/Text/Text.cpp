#include "Text.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"

using namespace DirectX;

ID3D11Device* Text::mDevice = nullptr;
ID3D11DeviceContext* Text::mDeviceContext = nullptr;
std::unique_ptr<TextResource> Text::mTextResource = nullptr;

Text::Text()
{
	mFontDataID = mTextResource->LoadFontData("Resource/BasicFontData.font");
}

Text::Text(const char* fontDataName, const size_t materialID)
	: mMaterialID(materialID)
{
	ASSERT(fontDataName != nullptr, "The fontDataName must not be null");
	ASSERT(mTextResource != nullptr, "The TextResource must not be null");

	mFontDataID = mTextResource->LoadFontData(fontDataName);
}

Text::~Text()
{
	RELEASE_COM(mSentenceVertexBuffer);
}

void Text::SetSentence(const char* sentence)
{
	ASSERT(sentence != nullptr, "The sentence must not be null");

	const size_t sentenceLength = strlen(sentence);

	/*
		기준 최대치를 넘는 경우 인자로 넘겨 받은 문장의 길이에서 반을 더 더한 값을 최대치로 지정하고 
		그 길이 만큼 버텍스 버퍼를 새로 만듭니다.
	*/
	if (sentenceLength > mMaxSentenceLength)
	{
		mMaxSentenceLength = sentenceLength + static_cast<size_t>(sentenceLength * 0.5f);

		// 기존에 있던 정점 버퍼는 해제합니다.
		RELEASE_COM(mSentenceVertexBuffer);
		
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(Vertex) * mMaxSentenceLength * 6;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HR(mDevice->CreateBuffer(&bufferDesc, nullptr, &mSentenceVertexBuffer));
	}

	unsigned int index = 0;
	auto vertices = std::make_unique<Vertex[]>(sentenceLength * 6);

	// 인자로 넘겨 받은 문장에 맞게 정점 버퍼를 구성합니다.
	{
		const TextResource::FontType* fontData = &mTextResource->GetFontData(mFontDataID);

		XMFLOAT2 drawingPosition = {};

		for (size_t i = 0; i < sentenceLength; ++i)
		{
			const char letter = sentence[i];

			if (letter == ' ')
			{
				drawingPosition.x += 3.0f;

				continue;
			}

			const TextResource::FontType& fontType = fontData[letter - 32];

			// Initialize first triangle in quad.
			vertices[index].Position = XMFLOAT4(drawingPosition.x, drawingPosition.y, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.leftU, 0.0f);
			++index;

			vertices[index].Position = XMFLOAT4(drawingPosition.x + fontType.pixelWidth, drawingPosition.y - 16, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.rightU, 1.0f);
			++index;

			vertices[index].Position = XMFLOAT4(drawingPosition.x, drawingPosition.y - 16, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.leftU, 1.0f);
			++index;

			// Initialize second triangle in quad.
			vertices[index].Position = XMFLOAT4(drawingPosition.x, drawingPosition.y, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.leftU, 0.0f);
			++index;

			vertices[index].Position = XMFLOAT4(drawingPosition.x + fontType.pixelWidth, drawingPosition.y, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.rightU, 0.0f);
			++index;

			vertices[index].Position = XMFLOAT4(drawingPosition.x + fontType.pixelWidth, drawingPosition.y - 16, 0.0f, 1.0f);
			vertices[index].TextureCoord = XMFLOAT2(fontType.rightU, 1.0f);
			++index;

			drawingPosition.x += fontType.pixelWidth + 1.0f;
		}

		mVertexCount = index;
	}

	// 구성한 정점 버퍼를 GPU 메모리에 적용시킵니다.
	{
		D3D11_MAPPED_SUBRESOURCE mapResource;
		HR(mDeviceContext->Map(mSentenceVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));

		memcpy(mapResource.pData, reinterpret_cast<void*>(vertices.get()), sizeof(Vertex) * mVertexCount);

		mDeviceContext->Unmap(mSentenceVertexBuffer, 0);
	}
}

void Text::_Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
	mTextResource = std::make_unique<TextResource>(device, deviceContext);
}

void Text::_Draw(RendererKey)
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mSentenceVertexBuffer, &stride, &offset);

	mDeviceContext->Draw(mVertexCount, 0);
}
