#pragma once

class Setting final
{
public:
	Setting(const Setting&) = delete;

	Setting& operator=(const Setting&) = delete;

	static inline Setting& Get()
	{
		static Setting setting;
		return setting;
	}

	inline int GetWidth() const { return 1280; }

	inline int GetHeight() const { return 720; }

	inline bool IsWindow() const { return true; }

	inline bool IsVsync() const { return true; }

private:
	Setting() = default;

	~Setting() = default;
};