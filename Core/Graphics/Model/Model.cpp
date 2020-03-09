#include "Model.h"
#include "../../../Common/Define.h"

Model::Model(const int materialCount)
{
	#if defined(DEBUG) | defined(_DEBUG)
	mMaterialCount = materialCount;
	#endif

	mTextureIndices = std::make_unique<int[]>(materialCount);
}

bool Model::IsActive() const
{
	return mbActive;
}

const DirectX::XMFLOAT3& Model::GetPosition() const
{
	return mPosition;
}

const DirectX::XMFLOAT3& Model::GetScale() const
{
	return mScale;
}

const DirectX::XMFLOAT3& Model::GetRotation() const
{
	return mRotation;
}

int Model::GetTextureIndex(const int materialIndex)
{
	return mTextureIndices[materialIndex];
}

void Model::SetActive(const bool bActive)
{
	mbActive = bActive;
}

void Model::SetPosition(const DirectX::XMFLOAT3& position)
{
	mPosition = position;
}

void Model::SetScale(const DirectX::XMFLOAT3& scale)
{
	mScale = scale;
}

void Model::SetRotation(const DirectX::XMFLOAT3& rotation)
{
	mRotation = rotation;
}

void Model::SetTextureIndex(const int materialIndex, const int textureIndex)
{
	ASSERT(0 <= materialIndex && materialIndex < mMaterialCount
		, "머티리얼 인덱스는 0보다 크거나 모델이 갖는 메쉬 개수보다 작아야 합니다.");

	mTextureIndices[materialIndex] = textureIndex;
}
