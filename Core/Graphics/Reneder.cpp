#include <algorithm>

#include "Reneder.h"
#include "../../Common/Define.h"
#include "../../common/DirectXMath.h"

using namespace DirectX;

Reneder::Reneder()
{
}

Reneder::~Reneder()
{
	for (auto& i : mMeshes)
	{
		RELEASE(i);
	}
}

void Reneder::InitializeManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;

	mCamera = std::make_unique<Camera>();
	
	Mesh::Initialize(mDevice, mDeviceContext);
	Material::Initialize(mDevice, mDeviceContext);
}

void Reneder::SortMesh()
{
	// 머티리얼을 한 번에 패스하기 위해 머티리얼 아이디를 기준으로 메쉬를 정렬합니다.
	std::sort(begin(mMeshes), end(mMeshes), [](const Mesh* a, const Mesh* b)
		{
			return a->GetMaterialID() < b->GetMaterialID();
		});
}

void Reneder::Draw()
{
	XMMATRIX matViewProjection;
	mCamera->LoadViewMatrix(&matViewProjection);

	static int targetMaterialID = 0;
	static Material* targetMaterial = nullptr;

	for (const auto& mesh : mMeshes)
	{
		const XMFLOAT3& meshPosition = mesh->GetPosition();
		const XMMATRIX matWorld = XMMatrixTranslation(meshPosition.x, meshPosition.y, meshPosition.z);

		// 현재 메쉬가 가지는 머티리얼 아이디가 이전 아이디와 달라지는 경우만 머티리얼을 활성화시킴으로써 성능을 향상시킵니다.
		if (targetMaterialID != mesh->GetMaterialID())
		{
			targetMaterialID = mesh->GetMaterialID();
			ASSERT(targetMaterialID > 0, "메쉬에 등록된 머티리얼이 없습니다.");

			targetMaterial = mMaterials.at(targetMaterialID).get();
			targetMaterial->Activate();
		}

		// 타겟 머티리얼의 worldViewProjection matrix를 업데이트합니다.
		targetMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
		targetMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection));

		mesh->Draw();
	}
}

void Reneder::AddMesh(Mesh* mesh)
{
	ASSERT(mesh != nullptr, "The mesh must not be null");
	mMeshes.push_back(mesh);
}

size_t Reneder::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");
	
	const size_t materialID = reinterpret_cast<size_t>(material);
	mMaterials.insert(std::make_pair(materialID, std::unique_ptr<Material>(material)));

	return materialID;
}
