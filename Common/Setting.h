#pragma once

class Setting final
{
public:
	Setting(const Setting&) = delete;

	Setting& operator=(const Setting&) = delete;

	static Setting& Get();

	int GetWidth() const;

	int GetHeight() const;

	bool IsWindow() const;

	bool IsVsync() const;

private:
	Setting() = default;

	~Setting() = default;
};