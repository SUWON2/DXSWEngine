#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "../../../Common/Define.h"
#include "../../../Common/DirectXTex.h"
#include "../../../Common/DirectXMath.h"

class MaterialResource final
{
public:
	explicit MaterialResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	MaterialResource(const MaterialResource&) = delete;

	MaterialResource& operator=(const MaterialResource&) = delete;

	~MaterialResource();

	ID LoadVertexShader(const char* fileName);
	
	ID LoadPixelShader(const char* fileName);

	ID LoadSingleTexture(const std::string& fileName);

	ID LoadTextureArray(const DirectX::XMINT2& size, const std::vector<std::string>& fileNames);

	const std::string& GetResourceName(const ID id) const;

	const std::pair<ID3D11VertexShader*, ID3D11InputLayout*>& GetVertexShaderBuffer(const ID id) const;

	ID3D11PixelShader* GetPixelShader(const ID id) const;

	ID3D11ShaderResourceView* GetSrvTexture(const ID id) const;

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// vertex shader name, vertex shader
	std::unordered_map<std::string, std::pair<ID3D11VertexShader*, ID3D11InputLayout*>> mVertexShaderBuffers;

	// pixel shader name, pixel shader
	std::unordered_map<std::string, ID3D11PixelShader*> mPixelShaders;

	// texture name id, { texture, shader resource view textuer }
	std::unordered_map<std::string, std::pair<ID3D11Texture2D*, ID3D11ShaderResourceView*>> mSingleTextures;

	// texture array
	std::unordered_set<ID3D11ShaderResourceView*> mTextureArrays;
};