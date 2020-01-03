#pragma once

#include <memory>
#include <d3d11.h>

#include "../../Common/Define.h"
#include "Camera/Camera.h"
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

	inline Camera* GetCamera() const
	{
		return mCamera.get();
	}

private:
	// HACk: 셰이더 구조를 정할 때 까지만 임시적으로 처리한다.
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11InputLayout* mInputLayout = nullptr;
	ID3D11PixelShader* mPixelShader = nullptr;
	ID3D11Buffer* mConstantBuffer = nullptr;

	std::unique_ptr<Camera> mCamera = nullptr;

	std::unique_ptr<MeshManager> mMeshManager = nullptr;
};