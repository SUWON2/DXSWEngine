#pragma once

// HACK: ���� ����� �ϴ��� �������
class RendererKey
{
	friend class Renderer;

	friend class SkyDome;

private:
	RendererKey() = delete;

	RendererKey(const RendererKey&) = default;

	RendererKey& operator=(const RendererKey&) = delete;

	~RendererKey() = default;
};