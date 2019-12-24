#pragma once

class Setting final
{
public:
	Setting(const Setting&) = delete;

	Setting& operator=(const Setting&) = delete;

	static Setting& Get()
	{
		static Setting setting;
		return setting;
	}

	inline int GetWidth() { return 800; }

	inline int GetHeight() { return 600; }

	inline bool IsVsync() { return true; }

private:
	Setting() = default;

	~Setting() = default;
};