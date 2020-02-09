#pragma once

#include <d3d11.h>
#include <string>
#include <unordered_map>

#include "../../../Common/Define.h"

class TextResource final
{
public:
	struct FontLetter
	{
		float leftU;
		float rightU;
		int pixelWidth;
	};

public:
	TextResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	TextResource(const TextResource&) = delete;

	TextResource& operator=(const TextResource&) = delete;

	~TextResource() = default;

	// return font data id
	ID LoadFontData(const char* fileName);

	const std::string& GetResourceName(const ID id) const;

	const FontLetter& GetFontLetter(const ID id) const;

private:
	ID3D11Device* mDevice = nullptr;

	ID3D11DeviceContext* mDeviceContext = nullptr;

	// font data name, font data
	std::unordered_map<std::string, std::unique_ptr<FontLetter[]>> mFontLetters;
};