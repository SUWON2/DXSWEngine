#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

#include "../../../Common/Define.h"

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

	ID LoadVertexShader(const char* fileName);
	
	ID LoadPixelShader(const char* fileName);

	ID LoadTexture(const char* fileName);

	const std::string& GetResourceName(const ID id) const;

	const VertexShaderBuffer& GetVertexShaderBuffer(const ID id) const;

	ID3D11PixelShader* GetPixelShader(const ID id) const;

	ID3D11ShaderResourceView** GetTexture(const ID id);

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