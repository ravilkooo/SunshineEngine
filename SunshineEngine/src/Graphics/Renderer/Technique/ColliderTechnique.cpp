#include "Graphics/Renderer/Technique/ColliderTechnique.h"
#include <Utils/StringUtils.h>
#include <Graphics/Renderer/Pass/ColliderPass.h>
#include <Graphics/GraphicsResources/VertexShader.h>

namespace SE_G {
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_boxShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_sphereShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_capsuleShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_taperedCapsuleShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_customShader;
	bool ColliderTechnique::s_staticDataInitializated = false;

	ColliderTechnique::ColliderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
		eastl::string technique, eastl::shared_ptr<SE::ColliderData> colliderData)
		: RenderTechnique(device, assignedTransform, technique)
	{
		if (!s_staticDataInitializated) {
			ColliderTechnique::InitStaticData(device);
		}
		m_colliderData = colliderData;

		m_transformCB = eastl::make_unique<Bind::VertexConstantBuffer<SE::ColliderTransformCB>>(
			device,
			SE::ColliderTransformCB{ DXSM::Matrix::Identity },
			2u);

		m_settingsCB = eastl::make_unique<Bind::VertexConstantBuffer<SE::ColliderSettings>>(
			device,
			m_colliderData->m_settings,
			3u);
	}

	void ColliderTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void ColliderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		UpdateTransformCB(context.Get());
		UpdateSettingsCB(context.Get());

		m_transformCB->Bind(context.Get());
		m_settingsCB->Bind(context.Get());

		switch (m_colliderData->m_shapeType)
		{
		case SE::ColliderShapeType::Box:
			s_boxShader->Bind(context.Get());
			break;

		case SE::ColliderShapeType::Sphere:
			s_sphereShader->Bind(context.Get());
			break;

		case SE::ColliderShapeType::Capsule:
			s_capsuleShader->Bind(context.Get());
			break;

		case SE::ColliderShapeType::TaperedCapsule:
			s_taperedCapsuleShader->Bind(context.Get());
			break;

		case SE::ColliderShapeType::Mesh:
			//m_customVertexBuffer->Bind(context.Get());
			s_customShader->Bind(context.Get());
			break;

		default:
			s_customShader->Bind(context.Get());
			break;
		}
	}

	void ColliderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		switch (m_colliderData->m_shapeType)
		{
		case SE::ColliderShapeType::Box:
		case SE::ColliderShapeType::Sphere:
		case SE::ColliderShapeType::Capsule:
		case SE::ColliderShapeType::TaperedCapsule:

			context->DrawIndexed(
				ColliderPass::s_shapeBufferOffsets[m_colliderData->m_shapeType].indexBufferSize,
				ColliderPass::s_shapeBufferOffsets[m_colliderData->m_shapeType].indexStart,
				0u
			);
			break;

		case SE::ColliderShapeType::Mesh:
		default:

			//s_customShader->Bind(context.Get());
			context->DrawIndexed(
				ColliderPass::s_shapeBufferOffsets[SE::ColliderShapeType::Box].indexBufferSize,
				ColliderPass::s_shapeBufferOffsets[SE::ColliderShapeType::Box].indexStart,
				0u
			);
			break;
		}

	}

	void ColliderTechnique::UpdateTransformCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		m_transformCB->Update(context.Get(), { m_colliderData->m_transformMat });
	}

	void ColliderTechnique::UpdateSettingsCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		m_settingsCB->Update(context.Get(), m_colliderData->m_settings);
	}

	void ColliderTechnique::InitStaticData(ID3D11Device* device)
	{
		UINT numInputElements = 1;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		s_boxShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/BoxVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_sphereShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/SphereVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		
		s_capsuleShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/CapsuleVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);

		s_taperedCapsuleShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/TaperedCapsuleVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_customShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/CustomVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_staticDataInitializated = true;
	}
}