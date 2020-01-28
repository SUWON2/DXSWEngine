#include <algorithm>

#include "Renderer.h"
#include "../../Common/Define.h"
#include "../../common/DirectXMath.h"

using namespace DirectX;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	for (auto& i : mTexts)
	{
		RELEASE(i);
	}

	for (auto& i : mModels)
	{
		RELEASE(i);
	}
}

void Renderer::InitializeManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ASSERT(device != nullptr, "The device must not be null");
	ASSERT(deviceContext != nullptr, "The deviceContext must not be null");

	mDevice = device;
	mDeviceContext = deviceContext;

	Model::_Initialize({}, mDevice, mDeviceContext);
	Text::_Initialize({}, mDevice, mDeviceContext);
	Material::_Initialize({}, mDevice, mDeviceContext);

	mCamera = std::make_unique<Camera>();
	mSkyDome = std::make_unique<SkyDome>();

	// ��Ʈ�� ���� �������� ���� �ؽ�Ʈ�� ���� �����ϴ� �⺻ ��Ʈ ��Ƽ������ �̸� �߰��մϴ�.
	Material* fontMaterial = Material::Create("Shaders/BasicFontShaderVS.hlsl", "Shaders/BasicFontShaderPS.hlsl");
	fontMaterial->RegisterTexture(0, "Resource/BasicFont.dds");
	mFontMaterialID = AddMaterial(fontMaterial);
}

void Renderer::SortText()
{
	// ��Ƽ������ �� ���� �н��ϱ� ���� ��Ƽ���� ���̵� �������� �ؽ�Ʈ�� �����մϴ�.
	{
		std::sort(begin(mTexts), end(mTexts), [](const Text* a, const Text* b)
			{
				return a->GetMaterialID() < b->GetMaterialID();
			});
	}
}

void Renderer::DrawSkyDome()
{
	const XMMATRIX matWorld =
		XMMatrixTranslation(mCamera->GetPosition().x, mCamera->GetPosition().y, mCamera->GetPosition().z);

	XMMATRIX matViewProjection;
	mCamera->LoadViewProjectionMatrix(&matViewProjection);

	mSkyDome->_Draw({}, matWorld, matViewProjection);
}

void Renderer::DrawModelAndText()
{
	// Draw models
	{
		XMMATRIX matViewProjection;
		mCamera->LoadViewProjectionMatrix(&matViewProjection);

		for (const auto& model : mModels)
		{
			if (model->IsActive() == false)
			{
				continue;
			}

			const XMMATRIX matWorld =
				XMMatrixScaling(model->GetScale().x, model->GetScale().y, model->GetScale().z)
				* XMMatrixRotationRollPitchYaw(XMConvertToRadians(model->GetRotation().x), XMConvertToRadians(model->GetRotation().y), XMConvertToRadians(model->GetRotation().z))
				* XMMatrixTranslation(model->GetPosition().x, model->GetPosition().y, model->GetPosition().z);

			for (unsigned int i = 0; i < model->GetMeshCount(); ++i)
			{
				ASSERT(model->GetMaterialIDs()[i] != 0
					, "��Ƽ���� ������ �޽� ������ �������� �ʽ��ϴ�. ��Ƽ������ �޽� ������ �°� ����� �ּ���");

				Material* material = mMaterials.at(model->GetMaterialIDs()[i]).get();
				material->_Activate({});

				material->UpdateBuffer(0, XMMatrixTranspose(matWorld));
				material->UpdateBuffer(1, XMMatrixTranspose(matViewProjection));

				model->_Draw({}, i);
			}
		}
	}

	// Draw texts
	{
		XMMATRIX matViewProjection2D;
		mCamera->LoadViewProjection2DMatrix(&matViewProjection2D);

		Material* currentMaterial = nullptr;

		for (const auto& text : mTexts)
		{
			if (text->IsActive() == false)
			{
				continue;
			}

			const XMMATRIX matWorld = XMMatrixTranslation(text->GetAnchorPoint().x + text->GetPosition().x
				, text->GetAnchorPoint().y + text->GetPosition().y, 0.0f);

			// ���� �ؽ�Ʈ�� ������ ��Ƽ���� ���̵� ���� ���̵�� �޶����� ��츸 ��Ƽ������ Ȱ��ȭ��Ŵ���ν� ������ ����ŵ�ϴ�.
			if (text->GetMaterialID() == 0
				&& reinterpret_cast<size_t>(currentMaterial) != mFontMaterialID)
			{
				currentMaterial = mMaterials.at(mFontMaterialID).get();
				currentMaterial->_Activate({});
			}
			else if (text->GetMaterialID() != 0
				&& reinterpret_cast<size_t>(currentMaterial) != text->GetMaterialID())
			{
				currentMaterial = mMaterials.at(text->GetMaterialID()).get();
				currentMaterial->_Activate({});
			}

			// Ÿ�� ��Ƽ������ worldViewProjection matrix�� ������Ʈ�մϴ�.
			currentMaterial->UpdateBuffer(0, XMMatrixTranspose(matWorld));
			currentMaterial->UpdateBuffer(1, XMMatrixTranspose(matViewProjection2D));

			text->_Draw({});
		}
	}
}

void Renderer::AddModel(Model* model)
{
	ASSERT(model != nullptr, "The model must not be null");

	#if defined(DEBUG) | defined(_DEBUG)
	for (const auto& i : mModels)
	{
		ASSERT(i != model, "There is already the model");
	}
	#endif

	mModels.push_back(model);
}

void Renderer::AddText(Text* text)
{
	ASSERT(text != nullptr, "The text must not be null");

	#if defined(DEBUG) | defined(_DEBUG)
	for (const auto& i : mTexts)
	{
		ASSERT(i != text, "There is already the text");
	}
	#endif

	mTexts.push_back(text);
}

size_t Renderer::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");

	const size_t materialID = reinterpret_cast<size_t>(material);
	ASSERT(mMaterials.find(materialID) == mMaterials.end(), "There is already the material");

	mMaterials.insert(std::make_pair(materialID, std::unique_ptr<Material>(material)));

	return materialID;
}
