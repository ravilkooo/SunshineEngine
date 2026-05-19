#include <Graphics/Renderer/Pass/PerceptionDebugPass.h>

#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/Bindable/Sampler.h>
#include <GameObject/GameObject.h>

#include <AI/Perception/PerceptionComponent.h>

#include <Utils/StringUtils.h>

namespace SE_G {

	PerceptionDebugPass::PerceptionDebugPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("PerceptionDebugPass", device, context)
	{
		eastl::vector<SE_G::PerceptionVertex> vertices;
		eastl::vector<UINT> indices;
		
		m_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Perception;

		// Set RTVs
		m_bufferRTVs[0] = pGBuffer->pLightRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;

		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_GBufferSampler = eastl::make_unique<Bind::Sampler>(device, samplerDesc, 0u);
		AddPerFrameBind(m_GBufferSampler.get());

		m_pixelShader = eastl::make_unique<Bind::PixelShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/PerceptionDebugPass/PerceptionPassPS.hlsl").c_str());

		AssetPath shaderPath = AssetPath(L"Shaders/PerceptionDebugPass/PerceptionPassVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		shaderPath.m_params.asShader.numInputElements = 1;
		shaderPath.m_params.asShader.IALayoutInputElements = new D3D11_INPUT_ELEMENT_DESC[shaderPath.m_params.asShader.numInputElements];
		shaderPath.m_params.asShader.IALayoutInputElements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle iconVshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* iconVshaderRes = rm.Get<SE_G::Bind::VertexShader>(iconVshaderHandle);
		m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			iconVshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;
		/*
		UINT numInputElements = 1;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		m_vertexShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/PerceptionDebugPass/PerceptionPassVS.hlsl").c_str(),
			numInputElements,
			IALayoutInputElements);

		m_settingsCB = eastl::make_unique<Bind::VertexConstantBuffer<PerceptionSettings>>(
			device,
			m_perceptionData,
			2u);
		*/

		InitVertexBuffer(device);
	}

	PerceptionDebugPass::~PerceptionDebugPass() {
		//delete[] m_bufferRTVs;
	}

	void PerceptionDebugPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Perception Pass");

		context->OMSetRenderTargets(2, m_bufferRTVs, m_GBuffer->pDepthDSV.Get());

		context->RSSetViewports(1, &m_viewport);

		m_camera->UpdateBuffer(context.Get());
		m_camera->BindBuffer(context.Get());
	}

	void PerceptionDebugPass::Pass()
	{
		if (m_gameObject && m_gameObject->HasComponent<PerceptionComponent_Info>()) 
		{
			PerceptionComponent_Info* perceptionComp = m_gameObject->GetComponent<PerceptionComponent_Info>().get();

			m_perceptionData.EyesOffset = perceptionComp->EyesOffset;
			m_perceptionData.SightRadius = perceptionComp->SightRadius;
			m_perceptionData.LoseRadius = perceptionComp->LoseRadius;
			m_perceptionData.FieldOfView = perceptionComp->FieldOfView;
			m_perceptionData.HearingRadius = perceptionComp->HearingRadius;

			auto tc = m_gameObject->GetComponent<TransformComponent_Info>();
			DXSM::Vector3 old_localScaleFactor = tc->m_assignedComponent->m_localScaleFactor;
			DXSM::Vector3 old_localRotation = tc->m_assignedComponent->m_localRotation;
			DXSM::Vector3 old_localPosition = tc->m_assignedComponent->m_localPosition;

			tc->m_assignedComponent->EnableMeshTransformMode();
			DXSM::Matrix fullTransform = tc->m_assignedComponent->GetWorldMatrix_noLocal();
			DXSM::Vector3 scale;
			DXSM::Vector3 rotate;
			DXSM::Vector3 translation;
			DecomposeTransform(fullTransform, scale, rotate, translation);
			tc->m_assignedComponent->m_localPosition = DXSM::Vector3::Zero;
			tc->m_assignedComponent->m_localRotation = DXSM::Vector3::Zero;
			tc->m_assignedComponent->m_localScaleFactor = DXSM::Vector3(
				1.0f / scale.x,
				1.0f / scale.y,
				1.0f / scale.z
			);

            const auto wMat = tc->m_assignedComponent->GetWorldMatrix_noLocal();
            DXSM::Matrix A = wMat;
            A._41 = 0; A._42 = 0; A._43 = 0; A._44 = 1;
            m_perceptionData.wMatNoLocalInvTranspose = (A.Invert()).Transpose();

			m_settingsCB->Update(context.Get(), m_perceptionData);
			m_settingsCB->Bind(context.Get());

			BindAllPerFrame();

			// VertexBuffer with all default shapes
			m_topology->Bind(context.Get());
			m_depthStencilState->Bind(context.Get());
			m_pixelShader->Bind(context.Get());
			m_vertexShader->Bind(context.Get());

			// Default shapes
			m_vertexBuffer->Bind(context.Get());
			m_indexBuffer->Bind(context.Get());


			tc->m_assignedComponent->BindToGraphicsPipeline(context.Get());
			// tc->Pass(GetDeviceContext());

			context->DrawIndexedInstanced(
				2 * segments + 4,
				3,
				0u,
				0u,
				0u
			);

			tc->m_assignedComponent->DisableMeshTransformMode();
			tc->m_assignedComponent->m_localScaleFactor = old_localScaleFactor;
			tc->m_assignedComponent->m_localRotation = old_localRotation;
			tc->m_assignedComponent->m_localPosition = old_localPosition;
		}
	}

	void PerceptionDebugPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(1, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void PerceptionDebugPass::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		// Set RTVs
		m_bufferRTVs[0] = m_GBuffer->pLightRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;
	}

	void PerceptionDebugPass::InitVertexBuffer(ID3D11Device* device)
	{

		eastl::vector<SE_G::PerceptionVertex> vertices;
		vertices.reserve(segments + 1u); // center + perimeter
		eastl::vector<UINT> indices;

		UINT vertexOffset = 0u;
		UINT indexOffset = 0u;

		vertices.push_back({DXSM::Vector3::Zero}); // center

		// 'segments + 1' vertices
		// 'segments' number of segments between vertices
		for (UINT i = 0u; i < segments + 1; ++i)
		{
			// float a = step * float(i);
			// float c = cosf(a);
			// float s = sinf(a);
			float c = 0.0f;
			float s = 1.0f;

			SE_G::PerceptionVertex v{};
			
			v.position = { 
				DXSM::Vector3(c, 0.0f, s)
			}; // XZ
			vertices.push_back(v);
		}

		// indices: connect i -> i+1, last -> first
		for (UINT i = 0u; i < vertices.size(); ++i)
		{
			UINT aIdx = UINT(i);
			UINT bIdx = UINT((i + 1u) % vertices.size());
			indices.push_back(aIdx);
			indices.push_back(bIdx);
		}

		m_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(
			device, vertices.data(), vertices.size(), sizeof(SE_G::PerceptionVertex));
		m_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(
			device, indices.data(), indices.size());
	}

	void PerceptionDebugPass::SetGameObject(GameObject_Info* gameObject) { m_gameObject = gameObject; }
}