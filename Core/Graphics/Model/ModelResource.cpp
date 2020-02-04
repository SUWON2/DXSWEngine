#include <vector>
#include <fstream>

#include "ModelResource.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"

using namespace DirectX;

ModelResource::ModelResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: mDevice(device)
	, mDeviceContext(deviceContext)
{
	ASSERT(mDevice != nullptr, "The device must not be null");
	ASSERT(mDeviceContext != nullptr, "The deviceContext must not be null");
}

ModelResource::~ModelResource()
{
	for (auto& modelData : mModelDatas)
	{
		for (auto& mesh : modelData.second)
		{
			RELEASE_COM(mesh.VertexBuffer);
		}
	}
}

size_t ModelResource::LoadVertexBuffer(const char* fileName)
{
	ASSERT(fileName != nullptr, "the fileName must not be null");

	// 이미 등록된 model data인 경우 더 이상 추가하지 않는다.
	const auto& foundModelData = mModelDatas.find(fileName);
	if (foundModelData != mModelDatas.end())
	{
		return reinterpret_cast<int>(&foundModelData->first);
	}

	struct F
	{
		int VIndex[3];
		int VtIndex[3];
		int VnIndex[3];
	};

	std::vector<XMFLOAT4> vList;
	std::vector<XMFLOAT2> vtList;
	std::vector<XMFLOAT3> vnList;
	std::vector<std::vector<F>> mList;
	
	// Load obj file data
	{
		XMFLOAT4 v;
		XMFLOAT2 vt;
		XMFLOAT3 vn;
		F f;

		std::ifstream in(fileName);
		ASSERT(in.is_open(), "Could not find a obj file");

		char type;
		while (in >> type)
		{
			if (type == 'v')
			{
				in >> std::noskipws >> type;
				in >> std::skipws;

				if (type == ' ')
				{
					in >> v.x >> v.y >> v.z;
					v.z *= -1.0f;
					v.w = 1.0f;

					vList.push_back(v);
				}
				else if (type == 't')
				{
					in >> vt.x >> vt.y;
					vt.y = 1.0f - vt.y;

					vtList.push_back(vt);
				}
				else if (type == 'n')
				{
					in >> vn.x >> vn.y >> vn.z;
					vnList.push_back(vn);
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

				for (int i = 0; i < 3; ++i)
				{
					in >> f.VIndex[i];
					--f.VIndex[i];
					in.get(); // read /

					in >> f.VtIndex[i];
					--f.VtIndex[i];
					in.get(); // read /

					in >> f.VnIndex[i];
					--f.VnIndex[i];
					in.get(); // read /
				}

				mList.back().push_back(f);
			}
			else if (type == 'm')
			{
				mList.push_back(std::vector<F>());
			}
		}

		in.close();
	}

	struct Vertex
	{
		XMFLOAT4 Position;
		XMFLOAT2 TextureCoord;
		XMFLOAT3 Normal;
	};

	std::vector<Mesh> modelData;
	Mesh mesh = {};

	for (const auto& m : mList)
	{
		mesh.VertexSize = sizeof(Vertex);
		mesh.VertexCount = m.size() * 3;

		auto vertices = std::make_unique<Vertex[]>(mesh.VertexCount);
		for (UINT i = 0; i < mesh.VertexCount; i += 3)
		{
			const F& f = m[i / 3];

			vertices[i].Position = vList[f.VIndex[2]];
			vertices[i].TextureCoord = vtList[f.VtIndex[2]];
			vertices[i].Normal = vnList[f.VnIndex[2]];

			vertices[i + 1].Position = vList[f.VIndex[1]];
			vertices[i + 1].TextureCoord = vtList[f.VtIndex[1]];
			vertices[i + 1].Normal = vnList[f.VnIndex[1]];

			vertices[i + 2].Position = vList[f.VIndex[0]];
			vertices[i + 2].TextureCoord = vtList[f.VtIndex[0]];
			vertices[i + 2].Normal = vnList[f.VnIndex[0]];
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(Vertex) * mesh.VertexCount;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pSysMem = vertices.get();

		HR(mDevice->CreateBuffer(&bufferDesc, &subResourceData, &mesh.VertexBuffer));

		modelData.push_back(mesh);
	}

	mModelDatas.insert(std::make_pair(fileName, std::move(modelData)));

	// model data id를 반환합니다.
	return reinterpret_cast<int>(&mModelDatas.find(fileName)->first);
}
