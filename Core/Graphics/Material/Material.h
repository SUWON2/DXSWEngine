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
	enum class ShaderType
	{
		VS, // vertex shader
		PS // pixel shader
	};

public:
	Material(const Material&) = delete;

	Material& operator=(const Material&) = delete;

	~Material();

	static Material* Create(const char* vertexShaderName, const char* pixelShaderName);

	template <typename T>
	void RegisterBuffer(const ShaderType shaderType, const unsigned int bufferIndex, const UINT bufferSize, const T& data);

	template <typename T>
	void UpdateBuffer(const ShaderType shaderType, const unsigned int bufferIndex, const T& data);

	void RegisterTexture(const unsigned int textureIndex, const char* fileName);

	const char* GetVertexShaderName() const;

	const char* GetPixelShaderName() const;

	const char* GetTextureName(const unsigned int index) const;

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void _Activate(RendererKey);

private:
	explicit Material(const char* vertexShaderName, const char* pixelShaderName);

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<MaterialResource> mMaterialResource;

	ID mVertexShaderId = 0;

	ID mPixelShaderId = 0;

	// texture index, texture id
	std::unordered_map<unsigned int, ID> mTextureIds;

	// buffer index, vertex shader constant buffer
	std::unordered_map<unsigned int, ID3D11Buffer*> mVSConstantBuffers;

	// buffer index, pixel shader constant buffer
	std::unordered_map<unsigned int, ID3D11Buffer*> mPSConstantBuffers;
};

template<typename T>
void Material::RegisterBuffer(const ShaderType shaderType, const unsigned int bufferIndex, const UINT bufferSize, const T& data)
{
	#if defined(DEBUG) | defined(_DEBUG)
	if (shaderType == ShaderType::VS && mVSConstantBuffers.size() > 1)
	{
		ASSERT(bufferIndex > 1, "항상 버텍스 셰이더 버퍼 인덱스 0과 1은 worldViewProjection에 사용되니 2 이상 버퍼 인덱스로만 등록해 주세요");
	}
	#endif

	ASSERT((shaderType == ShaderType::VS && mVSConstantBuffers.find(bufferIndex) == mVSConstantBuffers.end())
		|| (shaderType == ShaderType::PS && mPSConstantBuffers.find(bufferIndex) == mPSConstantBuffers.end())
		, "이미 사용되고 있는 버퍼 인덱스입니다. 다른 인덱스로 등록해 주세요");

	if ((shaderType == ShaderType::VS && mVSConstantBuffers.find(bufferIndex) != mVSConstantBuffers.end())
		|| (shaderType == ShaderType::PS && mPSConstantBuffers.find(bufferIndex) != mPSConstantBuffers.end()))
	{
		return;
	}

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = bufferSize;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;

	ID3D11Buffer* constantBuffer = nullptr;

	if constexpr (std::is_same<T, std::nullptr_t>::value)
	{
		HR(mDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
	}
	else
	{
		ASSERT(&data != nullptr, "The data address is null");

		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem = reinterpret_cast<const void*>(&data);

		HR(mDevice->CreateBuffer(&constantBufferDesc, &subResourceData, &constantBuffer));
	}

	if (shaderType == ShaderType::VS)
	{
		mVSConstantBuffers.insert(std::make_pair(bufferIndex, constantBuffer));
	}
	else
	{
		mPSConstantBuffers.insert(std::make_pair(bufferIndex, constantBuffer));
	}
}

template<typename T>
void Material::UpdateBuffer(const ShaderType shaderType, const unsigned int bufferIndex, const T& data)
{
	static_assert(std::is_same<T, std::nullptr_t>::value == false, "the T must not be null");

	ASSERT(&data != nullptr, "The data address is null");

	if (shaderType == ShaderType::VS)
	{
		mDeviceContext->UpdateSubresource(mVSConstantBuffers.at(bufferIndex), 0
			, nullptr, reinterpret_cast<const void*>(&data), 0, 0);
	}
	else
	{
		mDeviceContext->UpdateSubresource(mPSConstantBuffers.at(bufferIndex), 0
			, nullptr, reinterpret_cast<const void*>(&data), 0, 0);
	}
}