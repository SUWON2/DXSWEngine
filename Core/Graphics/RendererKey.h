#pragma once

// HACK: 무슨 기능을 하는지 명시하자
class RendererKey final
{
	friend class Renderer;

	friend class SkyDome;

private:
	RendererKey() = delete;

	RendererKey(const RendererKey&) = default;

	RendererKey& operator=(const RendererKey&) = delete;

	~RendererKey() = default;
};