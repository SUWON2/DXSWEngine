#pragma once

// HACK: 무슨 기능을 하는지 명시하자
class CoreKey final
{
private:
	friend class Core;

private:
	CoreKey() = delete;

	CoreKey(const CoreKey&) = default;

	CoreKey& operator=(const CoreKey&) = delete;

	~CoreKey() = default;
};