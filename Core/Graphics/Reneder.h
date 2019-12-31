#pragma once

#include <memory>
#include <d3d11.h>

#include "../../Common/Define.h"
#include "Mesh/MeshManager.h"

class Reneder final
{
public:
	Reneder();

	Reneder(const Reneder&) = default;

	Reneder& operator=(const Reneder&) = delete;

	~Reneder();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void Draw();

	inline void AddMesh(Mesh* mesh)
	{
		ASSERT(mesh != nullptr, "The mesh must not be null");
		mMeshManager->AddMesh(mesh);
	}

private:
	// HACk:
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;

	std::unique_ptr<MeshManager> mMeshManager = nullptr;
};