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

	// 폰트를 따로 지정하지 않은 텍스트를 위해 기본 폰트 머티리얼을 미리 추가하여 제공합니다.
	Material* fontMaterial = Material::Create("Shaders/BasicFontShaderVS.hlsl", "Shaders/BasicFontShaderPS.hlsl");
	fontMaterial->RegisterTexture(0, "Resource/BasicFont.dds");
	mBasicFontMaterialId = AddMaterial(fontMaterial);
}

void Renderer::SortText()
{
	// 머티리얼을 한 번에 패스하기 위해 머티리얼 아이디를 기준으로 텍스트를 정렬합니다.
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
					, "머티리얼 개수가 메쉬 개수와 대응되지 않습니다. 머티리얼 개수를 메쉬 개수에 맞게 등록해 주세요");

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

			// 현재 텍스트가 가지는 머티리얼 아이디가 이전 아이디와 달라지는 경우만 머티리얼을 활성화시킴으로써 성능을 향상시킵니다.
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

			// 타겟 머티리얼의 worldViewProjection matrix를 업데이트합니다.
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