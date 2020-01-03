#include <fstream>
#include <vector>

#include "MeshManager.h"
#include "../../../Common/Define.h"
#include "../../../Common/DirectXMath.h"
#include "Mesh.h"

using namespace DirectX;

MeshManager::MeshManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: mDevice(device)
	, mDeviceContext(deviceContext)
{
	ASSERT(mDevice != nullptr, "The device must not be null");
	ASSERT(mDeviceContext != nullptr, "The deviceContext must not be null");
}

MeshManager::~MeshManager()
{
	ReleaseCOM(mVertexBuffer);
}

void MeshManager::Draw()
{
	mDeviceContext->Draw(mVertexCount, 0);
}

void MeshManager::AddMesh(Mesh* mesh)
{
	ASSERT(mesh != nullptr, "The mesh must not be null");

	mMesh = std::unique_ptr<Mesh>(mesh);

	struct F
	{
		int VIndex[3];
		int VtIndex[3];
		int VnIndex[3];
	};

	std::vector<XMFLOAT3> vList;
	std::vector<XMFLOAT2> vtList;
	std::vector<XMFLOAT3> vnList;
	std::vector<F> fList;

	// Load obj data
	{
		XMFLOAT3 v;
		XMFLOAT2 vt;
		XMFLOAT3 vn;
		F f;

		std::ifstream in(mMesh->GetObjName());
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

					vList.push_back(v);
				}
				else if (type == 't')
				{
					in >> vt.x >> vt.y;
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
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

	mVertexCount = fList.size() * 3;
	auto vertices = std::make_unique<Vertex[]>(mVertexCount);

	for (int i = 0; i < mVertexCount; i += 3)
	{
		const F& f = fList[i / 3];

		vertices[i].Position = vList[f.VIndex[2]];
		vertices[i].Color = XMFLOAT4(vnList[f.VnIndex[2]].x, vnList[f.VnIndex[2]].y, vnList[f.VnIndex[2]].z, 1.0f);

		vertices[i + 1].Position = vList[f.VIndex[1]];
		vertices[i + 1].Color = XMFLOAT4(vnList[f.VnIndex[1]].x, vnList[f.VnIndex[1]].y, vnList[f.VnIndex[1]].z, 1.0f);

		vertices[i + 2].Position = vList[f.VIndex[0]];
		vertices[i + 2].Color = XMFLOAT4(vnList[f.VnIndex[0]].x, vnList[f.VnIndex[0]].y, vnList[f.VnIndex[0]].z, 1.0f);
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = vertices.get();

	HR(mDevice->CreateBuffer(&bufferDesc, &subResourceData, &mVertexBuffer));

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
