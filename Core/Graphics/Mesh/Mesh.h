#pragma once

#include <memory>
#include <string>

#include "MeshResource.h"
#include "../../../Common/DirectXMath.h"

class Mesh final
{
	friend class Reneder;

public:
	Mesh(const Mesh&) = delete;

	Mesh& operator=(const Mesh&) = delete;

	~Mesh();

	static inline Mesh* Create(const char* fileName)
	{
		return new Mesh(fileName);
	}

	static inline Mesh* Create(const char* fileName, const size_t materialID)
	{
		return new Mesh(fileName, materialID);
	}

	inline const char* GetFileName() const
	{
		return mMeshResource->GetResourceName(mVertexBufferID).c_str();
	}

	inline const size_t GetMaterialID() const
	{
		return mMaterialID;
	}

	inline const DirectX::XMFLOAT3& GetPosition() const
	{
		return mPosition;
	}

	inline void SetPosition(const DirectX::XMFLOAT3& position)
	{
		mPosition = position;
	}

	inline void SetMaterialID(const size_t materialID)
	{
		mMaterialID = materialID;
	}

private:
	explicit Mesh(const char* fileName);

	explicit Mesh(const char* fileName, const size_t materialID);

	static void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void Draw();

private:
	static ID3D11Device* mDevice;

	static ID3D11DeviceContext* mDeviceContext;

	static std::unique_ptr<MeshResource> mMeshResource;

	int mVertexBufferID = 0;

	size_t mMaterialID = 0;

	DirectX::XMFLOAT3 mPosition = {};
};