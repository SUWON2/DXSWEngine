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
	for (auto& i : mTexts)
	{
		RELEASE(i);
	}

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

	Mesh::Initialize({}, mDevice, mDeviceContext);
	Text::Initialize({}, mDevice, mDeviceContext);
	Material::Initialize({}, mDevice, mDeviceContext);

	// ��Ʈ�� ���� �������� ���� �ؽ�Ʈ�� ���� �����ϴ� �⺻ ��Ʈ ��Ƽ������ �̸� �߰��մϴ�.
	Material* fontMaterial = Material::Create("Shaders/BasicFontShaderVS.hlsl", "Shaders/BasicFontShaderPS.hlsl");
	fontMaterial->RegisterTexture(0, "Resource/BasicFont.dds");
	mFontMaterialID = AddMaterial(fontMaterial);
}

void Reneder::SortMeshAndText()
{
	// ��Ƽ������ �� ���� �н��ϱ� ���� ��Ƽ���� ���̵� �������� �޽��� �ؽ�Ʈ�� �����մϴ�.

	std::sort(begin(mMeshes), end(mMeshes), [](const Mesh* a, const Mesh* b)
		{
			return a->GetMaterialID() < b->GetMaterialID();
		});

	std::sort(begin(mTexts), end(mTexts), [](const Text* a, const Text* b)
		{
			return a->GetMaterialID() < b->GetMaterialID();
		});
}

void Reneder::Draw()
{
	// Draw meshes
	{
		XMMATRIX matViewProjection;
		mCamera->LoadViewProjectionMatrix(&matViewProjection);

		Material* currentMaterial = nullptr;

		for (const auto& mesh : mMeshes)
		{
			const XMMATRIX matWorld = XMMatrixScaling(mesh->GetScale().x, mesh->GetScale().y, mesh->GetScale().z)
				* XMMatrixTranslation(mesh->GetPosition().x, mesh->GetPosition().y, mesh->GetPosition().z);

			// ���� �޽��� ������ ��Ƽ���� ���̵� ���� ���̵�� �޶����� ��츸 ��Ƽ������ Ȱ��ȭ��Ŵ���ν� ������ ����ŵ�ϴ�.
			if (reinterpret_cast<size_t>(currentMaterial) != mesh->GetMaterialID())
			{
				currentMaterial = mMaterials.at(mesh->GetMaterialID()).get();
				currentMaterial->Activate({});
			}

			// Ÿ�� ��Ƽ������ worldViewProjection matrix�� ������Ʈ�մϴ�.
			currentMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
			currentMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection));

			mesh->Draw({});
		}
	}

	// Draw texts
	{
		XMMATRIX matViewProjection2D;
		mCamera->LoadViewProjection2DMatrix(&matViewProjection2D);

		Material* currentMaterial = nullptr;

		for (const auto& text : mTexts)
		{
			const XMMATRIX matWorld = XMMatrixTranslation(text->GetPosition().x, text->GetPosition().y, 0.0f);

			// ���� �ؽ�Ʈ�� ������ ��Ƽ���� ���̵� ���� ���̵�� �޶����� ��츸 ��Ƽ������ Ȱ��ȭ��Ŵ���ν� ������ ����ŵ�ϴ�.
			if (text->GetMaterialID() == 0
				&& reinterpret_cast<size_t>(currentMaterial) != mFontMaterialID)
			{
				currentMaterial = mMaterials.at(mFontMaterialID).get();
				currentMaterial->Activate({});
			}
			else if (text->GetMaterialID() != 0
				&& reinterpret_cast<size_t>(currentMaterial) != text->GetMaterialID())
			{
				currentMaterial = mMaterials.at(text->GetMaterialID()).get();
				currentMaterial->Activate({});
			}

			// Ÿ�� ��Ƽ������ worldViewProjection matrix�� ������Ʈ�մϴ�.
			currentMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
			currentMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection2D));

			text->Draw({});
		}
	}
}

size_t Reneder::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");

	const size_t materialID = reinterpret_cast<size_t>(material);
	mMaterials.insert(std::make_pair(materialID, std::unique_ptr<Material>(material)));

	return materialID;
}
