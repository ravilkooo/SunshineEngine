#include "Graphics/Renderer/Technique/ColliderTechnique.h"
#include <Utils/StringUtils.h>
#include <Graphics/Renderer/Pass/ColliderPass.h>

namespace SE_G {
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_planesSymmetryShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_yAxizSymmetryShader;
	eastl::unique_ptr<Bind::VertexShader> ColliderTechnique::s_customShader;
	bool ColliderTechnique::s_staticDataInitializated = false;

	ColliderTechnique::ColliderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
		eastl::string technique, SE::CollisionShape shape)
		: RenderTechnique(device, assignedTransform, technique)
	{
		if (!s_staticDataInitializated) {
			ColliderTechnique::InitStaticData(device);
		}
		m_shape = shape;
	}

	void ColliderTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void ColliderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		switch (m_shape)
		{
		case SE::CollisionShape::Box:
		case SE::CollisionShape::Sphere:
			s_planesSymmetryShader->Bind(context.Get());
			break;

		case SE::CollisionShape::Capsule:
			s_yAxizSymmetryShader->Bind(context.Get());
			break;

		case SE::CollisionShape::Mesh:
			//m_customVertexBuffer->Bind(context.Get());
			s_customShader->Bind(context.Get());
			break;

		default:
			s_customShader->Bind(context.Get());
			break;
		}

		//m_vertexBuffer->Bind(context.Get());
	}

	void ColliderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		switch (m_shape)
		{
		case SE::CollisionShape::Box:
		case SE::CollisionShape::Sphere:
		case SE::CollisionShape::Capsule:

			context->DrawIndexed(
				ColliderPass::s_shapeBufferOffsets[m_shape].indexBufferSize,
				ColliderPass::s_shapeBufferOffsets[m_shape].indexStart,
				0u
			);
			break;

		case SE::CollisionShape::Mesh:
		default:

			//s_customShader->Bind(context.Get());
			context->DrawIndexed(
				ColliderPass::s_shapeBufferOffsets[SE::CollisionShape::Box].indexBufferSize,
				ColliderPass::s_shapeBufferOffsets[SE::CollisionShape::Box].indexStart,
				0u
			);
			break;
		}

	}

	void ColliderTechnique::InitStaticData(ID3D11Device* device)
	{
		UINT numInputElements = 1;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		s_planesSymmetryShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/PlanesSymmetryVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_yAxizSymmetryShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/YAxizSymmetryVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_customShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/ColliderPass/CustomVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);
		s_staticDataInitializated = true;
	}
}