#pragma once

#include "../../../Common/DirectXMath.h"

class Camera final
{
public:
	Camera();

	Camera(const Camera&) = delete;

	Camera& operator=(const Camera&) = delete;

	~Camera();

	// TODO: �Ű����� �̸� �ٽ� ������ ����
	void MoveForward(const float velocity);

	void MoveX(const float velocity);

	void RotateY(const float angle);

	void RotateX(const float angle);

	void LoadViewMatrix(DirectX::XMMATRIX* matViewProjection);

	inline const DirectX::XMFLOAT3& GetPosition() const
	{
		return mPosition;
	}

	inline void SetPosition(const DirectX::XMFLOAT3 position)
	{
		mPosition = position;
	}

private:
	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mXAxis = { 1.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3 mYAxis = { 0.0f, 1.0f, 0.0f };

	DirectX::XMFLOAT3 mZAxis = { 0.0f, 0.0f, 1.0f };
};