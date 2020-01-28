#pragma once

#include <memory>

#include "../CoreKey.h"
#include "../Graphics/Renderer.h"
#include "../../Common/Define.h"

class Scene abstract
{
public:
	Scene();

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	virtual ~Scene();

	virtual void Initialize() = 0;

	virtual void Update(const float deltaTime) = 0;

	void AddModel(Model* model);

	void AddText(Text* text);

	size_t AddMaterial(Material* material);

	inline Camera* GetCamera() const
	{
		return mRenderer->GetCamera();
	}

	inline SkyDome* GetSkyDome() const
	{
		return mRenderer->GetSkyDome();
	}

public:
	inline Renderer* _GetRenderer(CoreKey) const
	{
		return mRenderer.get();
	}

private:
	std::unique_ptr<Renderer> mRenderer = nullptr;
};