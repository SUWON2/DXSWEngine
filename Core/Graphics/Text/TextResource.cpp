#include <fstream>

#include "TextResource.h"

TextResource::TextResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: mDevice(device)
	, mDeviceContext(deviceContext)
{
	ASSERT(mDevice != nullptr, "The device must not be null");
	ASSERT(mDeviceContext != nullptr, "The deviceContext must not be null");
}

ID TextResource::LoadFontData(const char* fileName)
{
	ASSERT(fileName != nullptr, "the fileName must not be null");

	// text가 가지는 font data가 이미 등록된 경우 더 이상 추가하지 않습니다.
	const auto& foundFontData = mFontLetters.find(fileName);
	if (foundFontData != mFontLetters.end())
	{
		return reinterpret_cast<ID>(&foundFontData->first);
	}

	std::ifstream in(fileName);
	ASSERT(in.is_open(), "Could not find a font data");

	auto fontData = std::make_unique<FontLetter[]>(95);

	// HACK: tutorial에서 제공하는 폰트 데이터만 가지고 테스트를 해봤기 때문에 더 다양한 폰트를 가지고 테스트한 뒤 정확히 처리하자
	for (int i = 0; i < 95; ++i)
	{
		int ascii;
		char letter;

		in >> ascii >> letter
			>> fontData[i].leftU >> fontData[i].rightU
			>> fontData[i].pixelWidth;
	}

	in.close();

	mFontLetters.insert(std::make_pair(fileName, std::move(fontData)));

	// vertex buffer id를 반환합니다.
	return reinterpret_cast<ID>(&mFontLetters.find(fileName)->first);
}

const std::string& TextResource::GetResourceName(const ID id) const
{
	return *reinterpret_cast<std::string*>(id);
}

const TextResource::FontLetter& TextResource::GetFontLetter(const ID id) const
{
	return *mFontLetters.at(GetResourceName(id)).get();
}