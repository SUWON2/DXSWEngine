#pragma once

#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "MaterialResource.h"
#include "../../Graphics/RendererKey.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"

class Material final
{
public:
	Material(const Material&) = delete;

	Material& operator=(const Material&) = delete;

	~Material();

	static Material* Create(const char* vertexShaderName, const char* pixelShaderName);

	template <typename T>
	void RegisterBuffer(const unsigned int bufferIndex, const size_t bufferSize, const T& data);

	template <typename T>
	inline void UpdateBuffer(const unsigned int bufferIndex, const T& data);

	void RegisterTexture(const unsigned int textureIndex, const char* fileName);

	inline const char* GetVertexShaderName() const
	{
		return mMaterialResource->GetResourceName(mVertexShaderID).c_str();
	}

	inline const char* GetPixelShaderName() const
	{
		return mMaterialResource->GetResourceName(mPixelShaderID).c_str();
	}

	inline const char* GetTextureName(const unsigned int index) const
	{
		return mMaterialResource->GetResourceName(mTextureIDs.at(index)).c_str();
	}

public:
	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	static void Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// �� �Լ��� ���� ���ο����� ���Ǵ� �Լ��Դϴ�.
	void Activate(RendererKey);

private:
	explicit Material(const char* vertexShaderName, const char* pixelShaderName);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<MaterialResource> mMaterialResource;

	size_t mVertexShaderID = 0;

	size_t  mPixelShaderID = 0;

	// texture index, texture id
	std::unordered_map<unsigned int, size_t> mTextureIDs;

	// buffer index, constant buffer
	std::unordered_map<unsigned int, ID3D11Buffer*> mConstantBuffers;
};

template<typename T>
inline void Material::RegisterBuffer(const unsigned int bufferIndex, const size_t bufferSize, const T& data)
{
	#if defined(DEBUG) | defined(_DEBUG)
	if (mConstantBuffers.size() > 1)
	{
		ASSERT(bufferIndex > 1, "�׻� ���� �ε��� 0�� 1�� worldViewProjection�� ���Ǵ� 2 �̻� ���� �ε����θ� ����� �ּ���");
	}
	#endif

	ASSERT(mConstantBuffers.find(bufferIndex) == mConstantBuffers.end()
		, "�̹� ���ǰ� �ִ� ���� �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

	if (mConstantBuffers.find(bufferIndex) != mConstantBuffers.end())
	{
		return;
	}

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = bufferSize;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;

	ID3D11Buffer* constantBuffer = nullptr;

	#if std::is_same<T, std::nullptr_t>::value
		HR(mDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
	#else
		ASSERT(&data != nullptr, "The data address is null");

		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem = reinterpret_cast<const void*>(&data);

		HR(mDevice->CreateBuffer(&constantBufferDesc, &subResourceData, &constantBuffer));
	#endif

	mConstantBuffers.insert(std::make_pair(bufferIndex, constantBuffer));
}

template<typename T>
inline void Material::UpdateBuffer(const unsigned int bufferIndex, const T& data)
{
	static_assert(std::is_same<T, std::nullptr_t>::value == false, "the T must not be null");

	ASSERT(&data != nullptr, "The data address is null");
	mDeviceContext->UpdateSubresource(mConstantBuffers.at(bufferIndex), 0
		, nullptr, reinterpret_cast<const void*>(&data), 0, 0);
}