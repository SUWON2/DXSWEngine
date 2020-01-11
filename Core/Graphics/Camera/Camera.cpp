#include "Camera.h"
#include "../../../Common/Define.h"
#include "../../../Common/Setting.h"

using namespace DirectX;

Camera::Camera()
{
}

Camera::~Camera()
{
}

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
	const XMMATRIX matRotationY = XMMatrixRotationY(angle);
	XMStoreFloat3(&mZAxis, XMVector3TransformNormal(XMLoadFloat3(&mZAxis), matRotationY));
	XMStoreFloat3(&mXAxis, XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMLoadFloat3(&mZAxis)));
}

void Camera::RotateX(const float angle)
{
	const XMMATRIX matRotationX = XMMatrixRotationAxis(XMLoadFloat3(&mXAxis), angle);
	XMStoreFloat3(&mZAxis, XMVector3TransformNormal(XMLoadFloat3(&mZAxis), matRotationX));
}

void Camera::LoadViewMatrix(DirectX::XMMATRIX* matViewProjection)
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
