#include <vector>
#include <fstream>

#include "MeshResource.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"

using namespace DirectX;

MeshResource::MeshResource(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: mDevice(device)
	, mDeviceContext(deviceContext)
{
	ASSERT(mDevice != nullptr, "The device must not be null");
	ASSERT(mDeviceContext != nullptr, "The deviceContext must not be null");
}

MeshResource::~MeshResource()
{
	for (auto& i : mVertexBuffers)
	{
		RELEASE_COM(i.second.Interface);
	}
}

size_t MeshResource::LoadVertexBuffer(const char* fileName)
{
	ASSERT(fileName != nullptr, "the fileName must not be null");

	// mesh가 가지는 obj가 이미 등록된 경우 더 이상 추가하지 않는다.
	const auto& foundVertexBuffer = mVertexBuffers.find(fileName);
	if (foundVertexBuffer != mVertexBuffers.end())
	{
		return reinterpret_cast<int>(&foundVertexBuffer->first);
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
	std::vector<F> fList;

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

				fList.push_back(f);
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

	VertexBuffer vertexBuffer;
	vertexBuffer.VertexSize = sizeof(Vertex);
	vertexBuffer.VertexCount = fList.size() * 3;

	auto vertices = std::make_unique<Vertex[]>(vertexBuffer.VertexCount);
	for (UINT i = 0; i < vertexBuffer.VertexCount; i += 3)
	{
		const F& f = fList[i / 3];

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
	bufferDesc.ByteWidth = sizeof(Vertex) * vertexBuffer.VertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = vertices.get();

	HR(mDevice->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer.Interface));

	mVertexBuffers.insert(std::make_pair(fileName, vertexBuffer));

	// vertex buffer id를 반환합니다.
	return reinterpret_cast<int>(&mVertexBuffers.find(fileName)->first);
}
