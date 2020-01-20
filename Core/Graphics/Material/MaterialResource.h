#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

class MaterialResource final
{
public:
	struct VertexShaderBuffer
	{
		ID3D11VertexShader* Interface;
		ID3D11InputLayout* InputLayout;
	};

public:
	explicit MaterialResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	MaterialResource(const MaterialResource&) = delete;

	MaterialResource& operator=(const MaterialResource&) = delete;

	~MaterialResource();

	size_t LoadVertexShader(const char* fileName);
	
	size_t LoadPixelShader(const char* fileName);

	size_t LoadTexture(const char* fileName);

	inline const std::string& GetResourceName(const size_t id) const
	{
		return *reinterpret_cast<std::string*>(id);
	}

	inline const VertexShaderBuffer& GetVertexShaderBuffer(const size_t id) const
	{
		return mVertexShaderBuffers.at(GetResourceName(id));
	}

	inline ID3D11PixelShader* GetPixelShader(const size_t id) const
	{
		return mPixelShaders.at(GetResourceName(id));
	}

	inline ID3D11ShaderResourceView** GetTexture(const size_t id)
	{
		return &mTextures.at(GetResourceName(id));
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// vertex shader name, vertex shader
	std::unordered_map<std::string, VertexShaderBuffer> mVertexShaderBuffers;

	// pixel shader name, pixel shader
	std::unordered_map<std::string, ID3D11PixelShader*> mPixelShaders;

	// texture name, texture
	std::unordered_map<std::string, ID3D11ShaderResourceView*> mTextures;
};