#include <algorithm>

#include "Renderer.h"
#include "../../Common/Define.h"
#include "../../common/DirectXMath.h"

using namespace DirectX;

Renderer::~Renderer()
{
	for (auto& i : mMaterials)
	{
		RELEASE(i);
	}

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

	// ��Ʈ�� ���� �������� ���� �ؽ�Ʈ�� ���� �⺻ ��Ʈ ��Ƽ������ �̸� �߰��Ͽ� �����մϴ�.
	Material* fontMaterial = Material::Create("Shaders/BasicFontShaderVS.hlsl", "Shaders/BasicFontShaderPS.hlsl");
	fontMaterial->RegisterTexture(0, "Resource/BasicFont.dds");
	mBasicFontMaterialId = AddMaterial(fontMaterial);
}

void Renderer::SortText()
{
	// ��Ƽ������ �� ���� �н��ϱ� ���� ��Ƽ���� ���̵� �������� �ؽ�Ʈ�� �����մϴ�.
	{
		std::sort(begin(mTexts), end(mTexts), [](const Text* a, const Text* b)
			{
				return a->GetMaterialId() < b->GetMaterialId();
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
				ASSERT(model->GetMaterialIds()[i] != 0
					, "��Ƽ���� ������ �޽� ������ �������� �ʽ��ϴ�. ��Ƽ���� ������ �޽� ������ �°� ����� �ּ���");

				Material* material = reinterpret_cast<Material*>(model->GetMaterialIds()[i]);
				material->_Activate({});

				material->UpdateBuffer<Material::ShaderType::VS>(0, XMMatrixTranspose(matWorld));
				material->UpdateBuffer<Material::ShaderType::VS>(1, XMMatrixTranspose(matViewProjection));

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
			if (text->GetMaterialId() == 0
				&& reinterpret_cast<ID>(currentMaterial) != mBasicFontMaterialId)
			{
				currentMaterial = reinterpret_cast<Material*>(mBasicFontMaterialId);
				currentMaterial->_Activate({});
			}
			else if (text->GetMaterialId() != 0
				&& reinterpret_cast<ID>(currentMaterial) != text->GetMaterialId())
			{
				currentMaterial = reinterpret_cast<Material*>(text->GetMaterialId());
				currentMaterial->_Activate({});
			}

			// Ÿ�� ��Ƽ������ worldViewProjection matrix�� ������Ʈ�մϴ�.
			currentMaterial->UpdateBuffer<Material::ShaderType::VS>(0, XMMatrixTranspose(matWorld));
			currentMaterial->UpdateBuffer<Material::ShaderType::VS>(1, XMMatrixTranspose(matViewProjection2D));

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

ID Renderer::AddMaterial(Material* material)
{
	ASSERT(material != nullptr, "The material must not be null");
	ASSERT(std::find(mMaterials.begin(), mMaterials.end(), material) == mMaterials.end(), "There is already the material");

	mMaterials.push_back(material);

	return  reinterpret_cast<ID>(material);
}

Camera* Renderer::GetCamera() const
{
	return mCamera.get();
}

SkyDome* Renderer::GetSkyDome() const
{
	return mSkyDome.get();
}