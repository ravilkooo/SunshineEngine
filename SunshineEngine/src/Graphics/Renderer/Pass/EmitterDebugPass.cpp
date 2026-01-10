#include <Graphics/Renderer/RenderingSystem.h>

#include <Graphics/Renderer/Pass/EmitterDebugPass.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/GraphicsResources/PixelShader.h>

#include <Graphics/Bindable/Sampler.h>
#include <Utils/StringUtils.h>

namespace SE_G {
	eastl::unique_ptr<Bind::Topology> EmitterDebugPass::s_topology;
	eastl::unique_ptr<Bind::IndexBuffer> EmitterDebugPass::s_indexBuffer;
	eastl::unique_ptr<Bind::VertexBuffer> EmitterDebugPass::s_vertexBuffer;

	bool EmitterDebugPass::s_staticDataInitializated = false;

	EmitterDebugPass::EmitterDebugPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("EmitterDebugPass", device, context)
	{
		if (!s_staticDataInitializated)
		{
			EmitterDebugPass::InitStaticData(device);
		}

		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Emitter;

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
			MakeEngineAssetPath_Wstring(L"Shaders/EmitterDebugPass/EmitterShaderPS.hlsl").c_str());
	}

	EmitterDebugPass::~EmitterDebugPass() {
		//delete[] m_bufferRTVs;
	}

	void EmitterDebugPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Emitter Pass");

		context->OMSetRenderTargets(2, m_bufferRTVs, m_GBuffer->pDepthDSV.Get());

		context->RSSetViewports(1, &m_viewport);

		m_camera->UpdateBuffer(context.Get());
		m_camera->BindBuffer(context.Get());
	}

	void EmitterDebugPass::Pass()
	{
		BindAllPerFrame();

		// VertexBuffer with all default shapes
		s_topology->Bind(context.Get());
		m_depthStencilState->Bind(context.Get());
		m_pixelShader->Bind(context.Get());

		// Default shapes
		s_vertexBuffer->Bind(context.Get());
		s_indexBuffer->Bind(context.Get());
		for (auto& tech : m_techniques)
		{
			DXSM::Vector3 old_scaleFactor = tech.second->m_assignedTransform->m_scaleFactor;
			tech.second->m_assignedTransform->m_scaleFactor = DXSM::Vector3::One;

			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());

			tech.second->m_assignedTransform->m_scaleFactor = old_scaleFactor;
		}
	}

	void EmitterDebugPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(1, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void EmitterDebugPass::OnResize(UINT resizeWidth, UINT resizeHeight)
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

	void EmitterDebugPass::InitStaticData(ID3D11Device* device)
	{
		eastl::vector<SE_G::EmitterVertex> vertices;
		eastl::vector<UINT> indices;

		vertices.reserve(0u);
		indices.reserve(0u);

		vertices.push_back(SE_G::EmitterVertex{ {0.0f, 0.0f, 0.0f} });
		vertices.push_back(SE_G::EmitterVertex{ {1.0f, 0.0f, 0.0f} });

		indices.push_back(0u);
		indices.push_back(1u);

		s_vertexBuffer = eastl::make_unique<Bind::VertexBuffer>(
			device, vertices.data(), vertices.size(), sizeof(SE_G::EmitterVertex));
		s_indexBuffer = eastl::make_unique<Bind::IndexBuffer>(
			device, indices.data(), indices.size());
		
		s_topology = eastl::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		s_staticDataInitializated = true;
	}
}