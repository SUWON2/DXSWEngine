#pragma once

// HACK: ���� ����� �ϴ��� �������
class CoreKey final
{
private:
	friend class Core;

public:
	CoreKey() = delete;

	CoreKey(const CoreKey&) = default;

	CoreKey& operator=(const CoreKey&) = delete;

	~CoreKey() = default;
};