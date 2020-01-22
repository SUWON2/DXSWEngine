#pragma once

#include <memory>

#include "../../Common/Define.h"
#include "../Graphics/Renderer.h"

class Scene abstract
{
	friend class Core;

public:
	Scene();

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	virtual ~Scene();

	virtual void Initialize() = 0;

	virtual void Update(const float deltaTime) = 0;

	inline void AddMesh(Mesh* mesh)
	{
		ASSERT(mesh != nullptr, "The mesh must not be null");
		mRenderer->AddMesh(mesh);
	}

	inline void AddText(Text* text)
	{
		ASSERT(text != nullptr, "The text must not be null");
		mRenderer->AddText(text);
	}

	inline size_t AddMaterial(Material* material)
	{
		ASSERT(material != nullptr, "The material must not be null");
		return mRenderer->AddMaterial(material);
	}

	inline Camera* GetCamera() const
	{
		return mRenderer->GetCamera();
	}

	inline SkyDome* GetSkyDome() const
	{
		return mRenderer->GetSkyDome();
	}

private:
	inline Renderer* GetRenderer() const
	{
		return mRenderer.get();
	}

private:
	std::unique_ptr<Renderer> mRenderer = nullptr;
};