#include <vector>
#include <unordered_map>
#include <fstream>

#include "ModelResource.h"

using namespace DirectX;

ModelResource::~ModelResource()
{
	for (auto& modelData : mModelDatas)
	{
		for (auto& mesh : modelData.second)
		{
			RELEASE_COM(mesh.IndexBuffer);
			RELEASE_COM(mesh.VertexBuffer);
		}
	}
}

void ModelResource::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;
}

ID ModelResource::LoadVertexBuffer(const char* fileName)
{
	ASSERT(fileName != nullptr, "the fileName must not be null");
	ASSERT(mDevice != nullptr, "The mDevice must not be null");
	ASSERT(mDeviceContext != nullptr, "The mDeviceContext must not be null");

	// �̹� ��ϵ� model data�� ��� �� �̻� �߰����� �ʴ´�.
	const auto& foundModelData = mModelDatas.find(fileName);
	if (foundModelData != mModelDatas.end())
	{
		return reinterpret_cast<ID>(&foundModelData->first);
	}

	std::ifstream in(fileName);
	ASSERT(in.is_open(), "Could not find a obj file");

	Vertex vertex;
	std::vector<XMFLOAT4> vList;
	std::vector<XMFLOAT2> vtList;
	std::vector<XMFLOAT3> vnList;

	// v, vt, vn�� �� �ε����� �����Ͽ� key�� �����ϰ� ���� �ε��� ������ �ε����� �����մϴ�.
	std::unordered_map<int, int> indexStorage;

	// indexStoreage�� key�� �����ϱ� ���� �������Դϴ�.
	int keyFirstOffset = 0;
	int keySecondOffset = 0;

	struct MeshBuffer
	{
		std::vector<Vertex> Vertices;
		std::vector<uint16_t> Indices;
	};
	std::vector<MeshBuffer> meshBuffers;

	char type;

	while (in >> type)
	{
		if (type == 'v')
		{
			in >> std::noskipws >> type;
			in >> std::skipws;

			if (type == ' ')
			{
				in >> vertex.Position.x 
					>> vertex.Position.y 
					>> vertex.Position.z;

				vertex.Position.z *= -1.0f;
				vertex.Position.w = 1.0f;

				vList.push_back(vertex.Position);
			}
			else if (type == 't')
			{
				in >> vertex.UV.x 
					>> vertex.UV.y;

				vertex.UV.y = 1.0f - vertex.UV.y;

				vtList.push_back(vertex.UV);
			}
			else if (type == 'n')
			{
				in >> vertex.Normal.x 
					>> vertex.Normal.y 
					>> vertex.Normal.z;

				vertex.Normal.z *= -1.0f;

				vnList.push_back(vertex.Normal);
			}
		}
		else if (type == 'f')
		{
			in >> std::noskipws >> type;
			in >> std::skipws;

			if (type != ' ')
			{
				continue;
			}

			int vIndices[3];
			int vtIndices[3];
			int vnIndices[3];

			for (int i = 0; i < 3; ++i)
			{
				in >> vIndices[i];
				--vIndices[i];
				in.get();

				in >> vtIndices[i];
				--vtIndices[i];
				in.get();

				in >> vnIndices[i];
				--vnIndices[i];
				in.get();
			}

			// �ﰢ���� ���� ������ �ٲߴϴ�. (������ ��ǥ�� -> �޼� ��ǥ��)
			std::swap(vIndices[0], vIndices[2]);
			std::swap(vtIndices[0], vtIndices[2]);
			std::swap(vnIndices[0], vnIndices[2]);

			for (int i = 0; i < 3; ++i)
			{
				const int vIndex = vIndices[i];
				const int vtIndex = vtIndices[i];
				const int vnIndex = vnIndices[i];

				int index = 0;
				MeshBuffer& meshBuffer = meshBuffers.back();

				const int key = vIndex + vtIndex * keyFirstOffset + vnIndex * keySecondOffset; // key ���� ��ġ�� �ʰ� �����մϴ�.
				const auto& foundIndex = indexStorage.find(key);

				// vertices�� indices�� ���� �����մϴ�.
				if (foundIndex != indexStorage.end())
				{
					index = foundIndex->second;
				}
				else
				{
					index = static_cast<int>(meshBuffer.Vertices.size());
					indexStorage.insert(std::make_pair(key, index));

					meshBuffer.Vertices.push_back({ vList[vIndex], vtList[vtIndex], vnList[vnIndex] });
				}

				meshBuffer.Indices.push_back(static_cast<uint16_t>(index));
			}
		}
		else if (type == 'm') // m�� mesh�� �����ϱ� ���� �뵵�� f�� ����ü�Դϴ�.
		{
			meshBuffers.push_back({});

			keyFirstOffset = vList.size() + 1;
			keySecondOffset = keyFirstOffset + keyFirstOffset * vtList.size();
		}
	}

	std::vector<Mesh> meshes;
	meshes.reserve(meshBuffers.size());

	D3D11_BUFFER_DESC bufferDesc = {};
	D3D11_SUBRESOURCE_DATA subResourceData = {};

	for (const auto& meshBuffer : meshBuffers)
	{
		bufferDesc.ByteWidth = sizeof(Vertex) * meshBuffer.Vertices.size();
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		subResourceData.pSysMem = meshBuffer.Vertices.data();

		ID3D11Buffer* vertexBuffer;
		HR(mDevice->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer));

		bufferDesc.ByteWidth = sizeof(uint16_t) * meshBuffer.Indices.size();
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subResourceData.pSysMem = meshBuffer.Indices.data();

		ID3D11Buffer* indexBuffer;
		HR(mDevice->CreateBuffer(&bufferDesc, &subResourceData, &indexBuffer));

		meshes.push_back({ vertexBuffer, indexBuffer, meshBuffer.Indices.size() });
	}

	mModelDatas.insert(std::make_pair(fileName, std::move(meshes)));

	// model data id�� ��ȯ�մϴ�.
	return reinterpret_cast<ID>(&mModelDatas.find(fileName)->first);
}

const std::string& ModelResource::GetResourceName(const ID id) const
{
	return *reinterpret_cast<std::string*>(id);
}

const std::vector<ModelResource::Mesh>& ModelResource::GetModelData(const ID id) const
{
	return mModelDatas.at(GetResourceName(id));
}