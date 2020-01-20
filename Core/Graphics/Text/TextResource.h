#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

class TextResource final
{
public:
	// HACK: 구조체명 다시 생각해 보자
	struct FontType
	{
		float leftU;
		float rightU;
		int pixelWidth;
	};

public:
	TextResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	TextResource(const TextResource&) = delete;

	TextResource& operator=(const TextResource&) = delete;

	~TextResource();

	// return font data id
	size_t LoadFontData(const char* fileName);

	inline const std::string& GetResourceName(const size_t id) const
	{
		return *reinterpret_cast<std::string*>(id);
	}

	inline const FontType& GetFontData(const size_t id) const
	{
		return *mFontDatas.at(GetResourceName(id)).get();
	}

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// font data name, font data
	std::unordered_map<std::string, std::unique_ptr<FontType[]>> mFontDatas;
};