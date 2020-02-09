#include "Camera.h"
#include "../../../Common/Define.h"
#include "../../../Common/Setting.h"

using namespace DirectX;

void Camera::MoveForward(const float velocity)
{
	mPosition =
	{
		mPosition.x + mZAxis.x * velocity,
		mPosition.y + mZAxis.y * velocity,
		mPosition.z + mZAxis.z * velocity,
	};
}

void Camera::MoveX(const float velocity)
{
	mPosition =
	{
		mPosition.x + mXAxis.x * velocity,
		mPosition.y + mXAxis.y * velocity,
		mPosition.z + mXAxis.z * velocity,
	};
}

void Camera::RotateY(const float angle)
{
	const XMMATRIX matRotationY = XMMatrixRotationY(XMConvertToRadians(angle));
	XMStoreFloat3(&mZAxis, XMVector3TransformNormal(XMLoadFloat3(&mZAxis), matRotationY));
	XMStoreFloat3(&mXAxis, XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMLoadFloat3(&mZAxis)));
}

void Camera::RotateX(const float angle)
{
	const XMMATRIX matRotationX = XMMatrixRotationAxis(XMLoadFloat3(&mXAxis), XMConvertToRadians(angle));
	XMStoreFloat3(&mZAxis, XMVector3TransformNormal(XMLoadFloat3(&mZAxis), matRotationX));
}

void Camera::LoadViewProjectionMatrix(DirectX::XMMATRIX* matViewProjection)
{
	ASSERT(matViewProjection != nullptr, "The matViewProjection must not be null");

	XMStoreFloat3(&mXAxis, XMVector3Normalize(XMLoadFloat3(&mXAxis)));
	XMStoreFloat3(&mZAxis, XMVector3Normalize(XMLoadFloat3(&mZAxis)));
	XMStoreFloat3(&mYAxis, XMVector3Cross(XMLoadFloat3(&mZAxis), XMLoadFloat3(&mXAxis)));

	const float dotX = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mPosition), XMLoadFloat3(&mXAxis)));
	const float dotY = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mPosition), XMLoadFloat3(&mYAxis)));
	const float dotZ = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mPosition), XMLoadFloat3(&mZAxis)));

	*matViewProjection =
	{
		mXAxis.x, mYAxis.x, mZAxis.x, 0.0f,
		mXAxis.y, mYAxis.y, mZAxis.y, 0.0f,
		mXAxis.z, mYAxis.z, mZAxis.z, 0.0f,
		dotX,	  dotY,	    dotZ,	  1.0f
	};

	const static XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4
		, Setting::Get().GetWidth() / static_cast<float>(Setting::Get().GetHeight()), 1.0f, 100.0f);

	*matViewProjection *= projection;
}

void Camera::LoadViewProjection2DMatrix(DirectX::XMMATRIX* matViewProjection2D)
{
	ASSERT(matViewProjection2D != nullptr, "The matViewProjection must not be null");

	const XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	*matViewProjection2D = XMMatrixLookAtLH(eye, at, up);

	const static XMMATRIX projection = XMMatrixOrthographicLH(static_cast<float>(Setting::Get().GetWidth())
		, static_cast<float>(Setting::Get().GetHeight()), 0.1f, 1000.0f);

	*matViewProjection2D *= projection;
}

const DirectX::XMFLOAT3& Camera::GetPosition() const
{
	return mPosition;
}

const DirectX::XMFLOAT3& Camera::GetViewDirection() const
{
	return mZAxis;
}

void Camera::SetPosition(const DirectX::XMFLOAT3 position)
{
	mPosition = position;
}
