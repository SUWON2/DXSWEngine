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
	// ��Ƽ������ �� ���� �н��ϱ� ���� ��Ƽ���� ���̵� �������� �޽��� �����մϴ�.
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

		// ���� �޽��� ������ ��Ƽ���� ���̵� ���� ���̵�� �޶����� ��츸 ��Ƽ������ Ȱ��ȭ��Ŵ���ν� ������ ����ŵ�ϴ�.
		if (targetMaterialID != mesh->GetMaterialID())
		{
			targetMaterialID = mesh->GetMaterialID();
			ASSERT(targetMaterialID > 0, "�޽��� ��ϵ� ��Ƽ������ �����ϴ�.");

			targetMaterial = mMaterials.at(targetMaterialID).get();
			targetMaterial->Activate();
		}

		// Ÿ�� ��Ƽ������ worldViewProjection matrix�� ������Ʈ�մϴ�.
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
