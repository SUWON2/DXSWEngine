#pragma once

// HACK: ���� ����� �ϴ��� �������
class RendererKey
{
	friend class Renderer;

private:
	RendererKey() = delete;

	RendererKey(const RendererKey&) = default;

	RendererKey& operator=(const RendererKey&) = delete;

	~RendererKey() = default;
};