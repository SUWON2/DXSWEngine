#pragma once

#include <string>

#include "../../../Common/DirectXMath.h"

class Mesh final
{
public:
	static inline Mesh* Create(const char* fileName)
	{
		return new Mesh(fileName);
	}

	Mesh(const Mesh&) = delete;

	Mesh& operator=(const Mesh&) = delete;

	~Mesh();

	inline const char* GetObjName() const
	{
		return mObjName.c_str();
	}

	inline const DirectX::XMFLOAT3& GetPosition() const
	{
		return mPosition;
	}

	inline void SetPosition(const DirectX::XMFLOAT3& position)
	{
		mPosition = position;
	}

private:
	explicit Mesh(const char* fileName);

private:
	std::string mObjName;

	DirectX::XMFLOAT3 mPosition = {};
};