#pragma once

// �Լ��� ���� ù ��° �Ű������� RendererKey�� ���������ν� Renderer Ŭ���� ���� �ܿ� �Լ� ȣ�⸦ ������ų �� �ֽ��ϴ�.
// ���� ù ��° �Ķ���͸� RendererKey�� ������ �Լ��� ���ξ� _�� ���Դϴ�.
class RendererKey final
{
	friend class Renderer;

	friend class SkyDome;

	friend class ModelFrame;

private:
	RendererKey() = delete;

	RendererKey(const RendererKey&) = default;

	RendererKey& operator=(const RendererKey&) = delete;

	~RendererKey() = default;
};