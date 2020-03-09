#include <algorithm>

#include "Renderer.h"
#include "../../Common/Define.h"
#include "../../common/DirectXMath.h"

using namespace DirectX;

Renderer::~Renderer()
{
	for (auto& i : mMaterials)
	{
		delete i.first;
	}

	for (auto& i : mTexts)
	{
		RELEASE(i);
	}

	for (auto& i : mModelFrames)
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

	ModelFrame::_Initialize({}, mDevice, mDeviceContext);
	Text::_Initialize({}, mDevice, mDeviceContext);
	Material::_Initialize({}, mDevice, mDeviceContext);

	mCamera = std::make_unique<Camera>();
	mSkyDome = std::make_unique<SkyDome>();

	// ��Ʈ�� ���� �������� ���� �ؽ�Ʈ�� ���� �⺻ ��Ʈ ��Ƽ������ �̸� �߰��Ͽ� �����մϴ�.
	mBasicFontMaterial = CreateMaterial("Shaders/BasicFontVS.hlsl", "Shaders/BasicFontPS.hlsl");
	mBasicFontMaterial->RegisterTexture(0, "Resource/BasicFont.dds");

	// HACK: Shadow
	Material* shadowMaterial = CreateMaterial("Shaders/DepthVS.hlsl", "Shaders/DepthPS.hlsl");
}

void Renderer::SortText()
{
	// ��Ƽ������ �� ���� �н��ϱ� ���� ��Ƽ���� ���̵� �������� �ؽ�Ʈ�� �����մϴ�.
	{
		std::sort(begin(mTexts), end(mTexts), [](const Text* a, const Text* b)
			{
				return a->GetMaterial() < b->GetMaterial();
			});
	}
}

Material* Renderer::CreateMaterial(const char* vertexShaderName, const char* pixelShaderName)
{
	ASSERT(vertexShaderName != nullptr, "The vertexShaderName must not be null");
	ASSERT(pixelShaderName != nullptr, "The pixelShaderName must not be null");

	Material* material = new Material({}, vertexShaderName, pixelShaderName, false);
	mMaterials.insert({ material, std::vector<std::pair<ModelFrame*, int>>() });

	return material;
}

ModelFrame* Renderer::CreateModelFrame(const char* fileName, const std::vector<Material*>& materials)
{
	ASSERT(fileName != nullptr, "The fileName must not be null");

	ModelFrame* modelFrame = new ModelFrame({}, fileName, materials);
	mModelFrames.push_back(modelFrame);

	// ��Ƽ������ �������� ���� �׸��� ����, model frame�� �� �޽��� �ε����� ��Ƽ���� �°� ��ġ�մϴ�.
	for (size_t i = 0; i < modelFrame->GetMaterialCount(); ++i)
	{
		const Material* material = modelFrame->GetMaterials()[i];
		mMaterials.at(material).push_back({ modelFrame, i });
	}

	return modelFrame;
}

Text* Renderer::CreateText()
{
	Text* text = new Text({});
	mTexts.push_back(text);

	return text;
}

void Renderer::DrawSkyDome()
{
	const XMMATRIX matWorld =
		XMMatrixTranslation(mCamera->GetPosition().x, mCamera->GetPosition().y, mCamera->GetPosition().z);

	XMMATRIX matViewProjection;
	mCamera->LoadViewProjectionMatrix(&matViewProjection);

	mSkyDome->_Draw({}, matWorld, matViewProjection);
}

void Renderer::DrawAllModel(ID3D11ShaderResourceView** shadowMap)
{
	// TODO: ���̿�, �׳� �Ϲ� �׸�������� �ؼ� �Լ� �и���Ű��
	const bool bDrawShadowMap = (shadowMap == nullptr);

	XMMATRIX matViewProjection;
	XMMATRIX matViewProjection2;

	// HACK: ���� ���������� ��û ���� �ػ󵵸� ����!
	// �� �� �� ��Ȯ�ϰ� ó���� �ʿ䰡 ���� ��
	constexpr float SHADOW_MAP_SIZE = 20.0f;

	XMFLOAT3 xAxis = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 yAxis = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 zAxis = { 0.0f, 0.0f, 1.0f };
	const XMMATRIX matRotationX = XMMatrixRotationAxis(XMLoadFloat3(&xAxis), XMConvertToRadians(50.0f));

	XMStoreFloat3(&zAxis, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&zAxis), matRotationX)));
	XMStoreFloat3(&xAxis, XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMLoadFloat3(&zAxis))));
	XMStoreFloat3(&yAxis, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&zAxis), XMLoadFloat3(&xAxis))));

	XMFLOAT3 position = { 24.0f, 10.0f, 30.0f };

	const float dotX = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&position), XMLoadFloat3(&xAxis)));
	const float dotY = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&position), XMLoadFloat3(&yAxis)));
	const float dotZ = -XMVectorGetX(XMVector3Dot(XMLoadFloat3(&position), XMLoadFloat3(&zAxis)));

	if (bDrawShadowMap)
	{
		matViewProjection =
		{
			xAxis.x, yAxis.x, zAxis.x, 0.0f,
			xAxis.y, yAxis.y, zAxis.y, 0.0f,
			xAxis.z, yAxis.z, zAxis.z, 0.0f,
			dotX,	 dotY,    dotZ,    1.0f
		};

		matViewProjection *= XMMatrixOrthographicLH(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1.0f, 50.0f);
	}
	else
	{
		mCamera->LoadViewProjectionMatrix(&matViewProjection);

		matViewProjection2 =
		{
			xAxis.x, yAxis.x, zAxis.x, 0.0f,
			xAxis.y, yAxis.y, zAxis.y, 0.0f,
			xAxis.z, yAxis.z, zAxis.z, 0.0f,
			dotX,	 dotY,    dotZ,    1.0f
		};

		matViewProjection2 *= XMMatrixOrthographicLH(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1.0f, 50.0f);
	}

	std::array<XMVECTOR, 6> planes;
	CreateFrustumPlanes(matViewProjection, &planes);

	// ��� model frame�� �ν��Ͻ� ���۸� ��� ������Ʈ�� �� ��Ƽ���� �����Ǵ� �޽��� ã�� �׸��ϴ�.
	{
		for (const auto& modelFrame : mModelFrames)
		{
			modelFrame->_UpdateInstanceBuffer({}, planes);
		}

		for (const auto& material : mMaterials)
		{
			// ���ǰ� ���� �ʴ� ��Ƽ������ �����մϴ�.
			if (material.second.empty())
			{
				continue;
			}

			material.first->_Active({}, nullptr, matViewProjection);

			for (const auto& modelFrame : material.second)
			{
				// TODO: ȭ�鿡 �� ���̴� ���� �����ϵ��� ó������.
				modelFrame.first->_DrawMesh({}, modelFrame.second);
			}
		}
	}
}

void Renderer::DrawAllText()
{
	XMMATRIX matViewProjection2D;
	mCamera->LoadViewProjection2DMatrix(&matViewProjection2D);

	Material* currentMaterial = nullptr;

	for (const auto& text : mTexts)
	{
		if (!text->IsActive())
		{
			continue;
		}

		const XMMATRIX matWorld = XMMatrixTranslation(text->GetAnchorPoint().x + text->GetPosition().x
			, text->GetAnchorPoint().y + text->GetPosition().y, 0.0f);

		// ���� �ؽ�Ʈ�� ������ ��Ƽ���� ���̵� ���� ���̵�� �޶����� ��츸 ��Ƽ������ Ȱ��ȭ��Ŵ���ν� ������ ����ŵ�ϴ�.
		if (text->GetMaterial() == nullptr && currentMaterial != mBasicFontMaterial)
		{
			currentMaterial = mBasicFontMaterial;
			currentMaterial->_Active({}, nullptr, XMMatrixTranspose(matViewProjection2D));
		}
		else if (text->GetMaterial() != nullptr && currentMaterial != text->GetMaterial())
		{
			currentMaterial = text->GetMaterial();
			currentMaterial->_Active({}, nullptr, XMMatrixTranspose(matViewProjection2D));
		}
		else
		{
			ASSERT(currentMaterial != nullptr, "The current must not be null");
		}

		// Ÿ�� ��Ƽ������ World matrix�� ������Ʈ�մϴ�.
		currentMaterial->UpdateBuffer<Material::ShaderType::VS>(0, XMMatrixTranspose(matWorld));

		text->_Draw({});
	}
}

Camera* Renderer::GetCamera() const
{
	return mCamera.get();
}

SkyDome* Renderer::GetSkyDome() const
{
	return mSkyDome.get();
}

void Renderer::CreateFrustumPlanes(const DirectX::XMMATRIX& matViewProjection, std::array<XMVECTOR, 6>* outPlanes)
{
	ASSERT(outPlanes != nullptr, "The outPlanes must not be null");

	XMVECTOR vertices[8] =
	{
		{ -1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
	};

	const XMMATRIX inverseMatViewProjection = XMMatrixInverse(nullptr, matViewProjection);

	for (int i = 0; i < 8; ++i)
	{
		vertices[i] = XMVector3TransformCoord(vertices[i], inverseMatViewProjection);
	}

	*outPlanes =
	{
		XMPlaneFromPoints(vertices[1], vertices[5], vertices[6]),
		XMPlaneFromPoints(vertices[4], vertices[0], vertices[3]),
		XMPlaneFromPoints(vertices[4], vertices[5], vertices[1]),
		XMPlaneFromPoints(vertices[3], vertices[2], vertices[6]),
		XMPlaneFromPoints(vertices[7], vertices[6], vertices[5]),
		XMPlaneFromPoints(vertices[0], vertices[1], vertices[2]),
	};
}
