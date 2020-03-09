#pragma once

#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "MaterialResource.h"
#include "../RendererKey.h"
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
	explicit Material(RendererKey, const char* vertexShaderName, const char* pixelShaderName, const bool bUseShadowMap = false);

	Material(const Material&) = delete;

	Material& operator=(const Material&) = delete;

	~Material();

	template <ShaderType shaderType, typename T>
	void RegisterBuffer(const unsigned int index, const UINT bufferSize, const T& data);

	template <ShaderType shaderType, typename T>
	void UpdateBuffer(const unsigned int index, const T& data) const;

	void RegisterTexture(const unsigned int index, const char* fileName);

	void RegisterTextureArray(const unsigned int index, const DirectX::XMINT2& size, const std::vector<std::string>& fileNames);

	const char* GetVertexShaderName() const;

	const char* GetPixelShaderName() const;

public:
	static void _Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	// TODO: �� ��° �Ķ���� ���� �����ͷ� ���� �� �ְ� ��������.
	void _Active(RendererKey, ID3D11ShaderResourceView** shadowMap, const DirectX::XMMATRIX& matViewProjection) const;

	// TODO: �Լ��� �ٽ� ��������.
	void _ActiveWithShadowMode(RendererKey);

	bool _IsUsedShadowMap() const;

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<MaterialResource> mMaterialResource;

	ID mVertexShaderId = 0;

	ID mPixelShaderId = 0;

	bool mbUseShadowMap = false;

	// buffer index, vertex shader constant buffer
	std::unordered_map<unsigned int, ID3D11Buffer*> mVSConstantBuffers;

	// buffer index, pixel shader constant buffer
	std::unordered_map<unsigned int, ID3D11Buffer*> mPSConstantBuffers;

	// texture index, texture resource id
	std::unordered_map<unsigned int, ID> mSingleTextureIds;

	// texture array index, texture array resource id
	std::unordered_map<unsigned int, ID> mTextureArrayIds;
};

template<Material::ShaderType shaderType, typename T>
void Material::RegisterBuffer(const unsigned int index, const UINT bufferSize, const T& data)
{
#if defined(DEBUG) | defined(_DEBUG)
	if constexpr (shaderType == ShaderType::VS)
	{
		if (mVSConstantBuffers.size() > 1)
		{
			ASSERT(index > 1, "���ؽ� ���̴��� ���� �ε��� 0, 1��°�� ���� ���ο��� worldViewProjection�� ���˴ϴ�. 2 �̻��� ���� �ε����θ� ����� �ּ���");
		}
	}
#endif

	ASSERT((shaderType == ShaderType::VS && mVSConstantBuffers.find(index) == mVSConstantBuffers.end())
		|| (shaderType == ShaderType::PS && mPSConstantBuffers.find(index) == mPSConstantBuffers.end())
		, "�̹� ���ǰ� �ִ� ���� �ε����Դϴ�. �ٸ� �ε����� ����� �ּ���");

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

	if constexpr (shaderType == ShaderType::VS)
	{
		mVSConstantBuffers.insert(std::make_pair(index, constantBuffer));
	}
	else
	{
		mPSConstantBuffers.insert(std::make_pair(index, constantBuffer));
	}
}

template <Material::ShaderType shaderType, typename T>
void Material::UpdateBuffer(const unsigned int index, const T& data) const
{
	static_assert(std::is_same<T, std::nullptr_t>::value == false, "the T must not be null");

	ASSERT(&data != nullptr, "The data address is null");

	if constexpr (shaderType == ShaderType::VS)
	{
		mDeviceContext->UpdateSubresource(mVSConstantBuffers.at(index), 0
			, nullptr, reinterpret_cast<const void*>(&data), 0, 0);
	}
	else
	{
		mDeviceContext->UpdateSubresource(mPSConstantBuffers.at(index), 0
			, nullptr, reinterpret_cast<const void*>(&data), 0, 0);
	}
}