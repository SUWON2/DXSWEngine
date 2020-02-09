#include "Setting.h"

Setting& Setting::Get()
{
	static Setting setting;
	return setting;
}

int Setting::GetWidth() const
{
	return 1280;
}

int Setting::GetHeight() const
{
	return 720;
}

bool Setting::IsWindow() const
{
	return true;
}

bool Setting::IsVsync() const
{
	return true;
}
