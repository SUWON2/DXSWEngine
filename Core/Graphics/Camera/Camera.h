#pragma once

#include "../../../Common/DirectXMath.h"

class Camera final
{
public:
	Camera() = default;

	Camera(const Camera&) = delete;

	Camera& operator=(const Camera&) = delete;

	~Camera() = default;

	// TODO: �Ű����� �̸� �ٽ� ������ ����
	void MoveForward(const float velocity);

	void MoveX(const float velocity);

	void RotateY(const float angle);

	void RotateX(const float angle);

	void LoadViewProjectionMatrix(DirectX::XMMATRIX* matViewProjection);

	void LoadViewProjection2DMatrix(DirectX::XMMATRIX* matViewProjection2D);

	const DirectX::XMFLOAT3& GetPosition() const;

	const DirectX::XMFLOAT3& GetViewDirection() const;

	void SetPosition(const DirectX::XMFLOAT3 position);

private:
	DirectX::XMFLOAT3 mPosition = {};

	DirectX::XMFLOAT3 mXAxis = { 1.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3 mYAxis = { 0.0f, 1.0f, 0.0f };

	DirectX::XMFLOAT3 mZAxis = { 0.0f, 0.0f, 1.0f };
};