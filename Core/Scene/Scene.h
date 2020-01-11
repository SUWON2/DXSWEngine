#pragma once

#include <memory>

#include "../../Common/Define.h"
#include "../Graphics/Reneder.h"

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
		mReneder->AddMesh(mesh);
	}

	inline size_t AddMaterial(Material* material)
	{
		ASSERT(material != nullptr, "The material must not be null");
		return mReneder->AddMaterial(material);
	}

	inline Camera* GetCamera() const
	{
		return mReneder->GetCamera();
	}

private:
	inline Reneder* GetReneder() const
	{
		return mReneder.get();
	}

private:
	std::unique_ptr<Reneder> mReneder = nullptr;
};