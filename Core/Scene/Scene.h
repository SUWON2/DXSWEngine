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

	virtual ~Scene() = default;

	virtual void Initialize() = 0;

	virtual void Update(const float deltaTime) = 0;

	void AddModel(Model* model);

	void AddText(Text* text);

	ID AddMaterial(Material* material);

	Camera* GetCamera() const;

	SkyDome* GetSkyDome() const;

public:
	Renderer* _GetRenderer(CoreKey) const;

private:
	std::unique_ptr<Renderer> mRenderer = nullptr;
};