#include <memory>

#include "ModelFrame.h"
#include "../../../Common/Define.h"

using namespace DirectX;

ID3D11Device* ModelFrame::mDevice = nullptr;
ID3D11DeviceContext* ModelFrame::mDeviceContext = nullptr;
std::unique_ptr<ModelResource> ModelFrame::mModelResource = nullptr;

ModelFrame::ModelFrame(RendererKey, const char* fileName, const std::vector<Material*>& materials)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");
	ASSERT(mModelResource != nullptr, "The modelResource must not be null");

	mModelId = mModelResource->LoadVertexBuffer(fileName);

	const size_t meshCount = mModelResource->GetModelData(mModelId).size();
	ASSERT(meshCount == materials.size()
		, "머티리얼 개수가 메쉬 개수와 맞지 않습니다. 메쉬 개수에 맞게 머티리얼을 등록해 주세요");

	mMaterials = { std::make_unique<Material * []>(meshCount), meshCount };
	memcpy(mMaterials.first.get(), materials.data(), sizeof(Material*) * meshCount);

	const size_t instanceBufferCount = meshCount + 1;
	mInstanceBuffers = { std::make_unique<ID3D11Buffer * []>(instanceBufferCount), instanceBufferCount };
	mMappedSubResource = std::make_unique<D3D11_MAPPED_SUBRESOURCE[]>(instanceBufferCount);
}

ModelFrame::~ModelFrame()
{
	for (size_t i = 0; i < mInstanceBuffers.second; ++i)
	{
		RELEASE_COM(mInstanceBuffers.first[i]);
	}

	for (auto& i : mModels)
	{
		RELEASE(i);
	}
}

void ModelFrame::Create(Model** outModel)
{
	ASSERT(outModel, "The outModel must not be null");

	*outModel = new Model(mMaterials.second);
	mModels.push_back(*outModel);

	const size_t previousCapacity = mModels.capacity();
	mModels.reserve(mModels.size() + 1);

	if (previousCapacity < mModels.capacity())
	{
		RemakeInstanceBuffer();
	}
}

void ModelFrame::Create(const int count, std::vector<Model*>* outModels)
{
	ASSERT(count > 0, "count는 1 이상 이어야 합니다");
	ASSERT(outModels != nullptr, "The outModels must not be null");
	ASSERT(outModels->empty() && outModels->capacity() == 0, "벡터가 비어 있어야 합니다.");

	outModels->reserve(count);

	const size_t previousCapacity = mModels.capacity();
	mModels.reserve(mModels.size() + count);

	for (int i = 0; i < count; ++i)
	{
		Model* model = new Model(mMaterials.second);
		outModels->push_back(model);
		mModels.push_back(model);
	}

	if (previousCapacity < mModels.capacity())
	{
		RemakeInstanceBuffer();
	}
}

Material** ModelFrame::GetMaterials() const
{
	return mMaterials.first.get();
}

size_t ModelFrame::GetMaterialCount() const
{
	return mMaterials.second;
}

void ModelFrame::_Initialize(RendererKey, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;

	mModelResource = std::make_unique<ModelResource>();
	mModelResource->Initialize(mDevice, mDeviceContext);
}

void ModelFrame::_UpdateInstanceBuffer(RendererKey, const std::array<DirectX::XMVECTOR, 6>& frustumPlanes)
{
	if (mModels.empty())
	{
		return;
	}

	for (size_t i = 0; i < mInstanceBuffers.second; ++i)
	{
		HR(mDeviceContext->Map(mInstanceBuffers.first[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mMappedSubResource[i]));
	}

	mDrawnCount = 0;

	for (const auto& model : mModels)
	{
		// 모델이 카메라 시야 밖에 있으면 무시합니다.
		{
			bool bEscape = false;

			for (const auto& plane : frustumPlanes)
			{
				// TODO: scale 정확하게 처리해야 됨
				constexpr float scale = 1.5f;

				if (XMVectorGetX(XMPlaneDotCoord(plane, XMVectorSet(model->GetPosition().x, model->GetPosition().y, model->GetPosition().z, 1.0f))) > scale)
				{
					bEscape = true;

					break;
				}
			}

			if (bEscape)
			{
				continue;
			}
		}

		// 인스턴스 버퍼들을 업데이트합니다.
		for (size_t i = 0; i < mInstanceBuffers.second; ++i)
		{
			if (i > 0)
			{
				auto instances = static_cast<MeshInstanceData*>(mMappedSubResource[i].pData);
				instances[mDrawnCount].textureIndex = model->GetTextureIndex(i - 1);
			}
			else
			{
				auto instances = static_cast<ModelInstanceData*>(mMappedSubResource[i].pData);

				const XMMATRIX matWorld =
					XMMatrixScaling(model->GetScale().x, model->GetScale().y, model->GetScale().z)
					* XMMatrixRotationRollPitchYaw(XMConvertToRadians(model->GetRotation().x), XMConvertToRadians(model->GetRotation().y), XMConvertToRadians(model->GetRotation().z))
					* XMMatrixTranslation(model->GetPosition().x, model->GetPosition().y, model->GetPosition().z);

				XMStoreFloat4x4(&instances[mDrawnCount].World, matWorld);
			}
		}

		++mDrawnCount;
	}

	for (size_t i = 0; i < mInstanceBuffers.second; ++i)
	{
		mDeviceContext->Unmap(mInstanceBuffers.first[i], 0);
	}
}

void ModelFrame::_DrawMesh(RendererKey, const int index)
{
	if (mModels.empty())
	{
		return;
	}

	const ModelResource::Mesh& mesh = mModelResource->GetModelData(mModelId).at(index);

	constexpr int instanceBufferCount = 3;
	uint32_t offsets[instanceBufferCount] = { 0, 0, 0 };
	uint32_t strides[instanceBufferCount] = { sizeof(ModelResource::Vertex), sizeof(ModelInstanceData), sizeof(MeshInstanceData) };
	ID3D11Buffer* buffers[instanceBufferCount] = { mesh.VertexBuffer, mInstanceBuffers.first[0], mInstanceBuffers.first[index + 1] };
	mDeviceContext->IASetVertexBuffers(0, instanceBufferCount, buffers, strides, offsets);

	mDeviceContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	mDeviceContext->DrawIndexedInstanced(mesh.IndexCount, mDrawnCount, 0, 0, 0);
}

void ModelFrame::RemakeInstanceBuffer()
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	for (size_t i = 0; i < mInstanceBuffers.second; ++i)
	{
		RELEASE_COM(mInstanceBuffers.first[i]);

		if (i > 0)
		{
			bufferDesc.ByteWidth = sizeof(MeshInstanceData) * mModels.capacity();
		}
		else
		{
			bufferDesc.ByteWidth = sizeof(ModelInstanceData) * mModels.capacity();
		}

		HR(mDevice->CreateBuffer(&bufferDesc, nullptr, &mInstanceBuffers.first[i]));
	}
}
