#include <Graphics/Renderer/Pass/IconPass.h>
#include <Utils/StringUtils.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

namespace SE_G {
	IconPass::IconPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("IconPass", device, context)
	{
		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;

		// Set RTVs
		m_bufferRTVs[0] = pGBuffer->pLightRTV.Get();
		m_bufferRTVs[1] = pGBuffer->pUUIDRTV.Get();

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
		m_depthStencilState = eastl::make_shared<Bind::DepthStencilState>(device, dsDesc);

		UINT numInputElements = 2;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_iconVertexShader = eastl::make_shared<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wchar(L"Shaders/IconPass/IconShaderVS.hlsl"),
			numInputElements, IALayoutInputElements);

		m_camGCB = eastl::make_shared<Bind::GeometryConstantBuffer<CamGCB>>
			(
				device,
				0u
			);

		AddPerFrameBind(m_camGCB.get());

		m_iconGeometryShader = eastl::make_shared<Bind::GeometryShader>(device,
			MakeEngineAssetPath_Wchar(L"Shaders/IconPass/IconShaderGS.hlsl"));

		m_iconSprites = eastl::make_shared<Bind::Texture>(
			device,
			MakeEngineAssetPath_Wstring(L"EditorIcons.dds"),
			0u,
			Bind::PipelineStage::PIXEL_SHADER
		);

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
		AddPerFrameBind(new Bind::Sampler(device, samplerDesc, 0u));

		m_spritesheetInfoPCB = eastl::make_shared<Bind::PixelConstantBuffer<SpritesheetInfoPCB>>(
			device,
			m_spritesheetData,
			0u);
		AddPerFrameBind(m_spritesheetInfoPCB.get());

		m_iconPixelShader = eastl::make_shared<Bind::PixelShader>(device,
			MakeEngineAssetPath_Wchar(L"Shaders/IconPass/IconShaderPS.hlsl"));
	}

	void IconPass::StartFrame()
	{
		context->OMSetRenderTargets(2, m_bufferRTVs, m_GBuffer->pDepthDSV.Get());

		context->RSSetViewports(1, &m_viewport);
	}

	void IconPass::Pass(const Scene& scene)
	{
		BindAllPerFrame();
		m_iconVertexShader->Bind(context.Get());
		m_iconGeometryShader->Bind(context.Get());
		m_iconPixelShader->Bind(context.Get());
		m_iconSprites->Bind(context.Get());
		m_depthStencilState->Bind(context.Get());

		m_spritesheetInfoPCB->Bind(context.Get());

		m_camGCB->Update(GetDeviceContext(), {
			m_camera->GetViewMatrix(),
			m_camera->GetProjectionMatrix(),
			m_camera->GetPosition(), 0.0f });
		m_camGCB->Bind(context.Get());

		for (const auto& gameObjectUUID : scene.gameObjects) {
			const auto& gameObject = scene.GetGameObjectByUUID(gameObjectUUID);
			if (gameObject->HasComponent<RenderComponent>() &&
				gameObject->HasComponent<TransformComponent>()) {

				auto renderComponent = gameObject->GetComponent<RenderComponent>();

				if (!renderComponent->HasTechnique(techniqueTag))
					continue;

				// m_geometryCB->Update(context.Get(), gameObject->GetComponent<TransformComponent>()->m_position);
				// m_geometryCB->Bind(context.Get());
				gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());

				renderComponent->PassTechnique(techniqueTag, GetDeviceContext()); // Bind + Draw
			}
		}

	}

	void IconPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(2, nullRTVs, *nullDSVs);
	}

	void IconPass::OnResize(UINT resizeWidth, UINT resizeHeight)
		//eastl::shared_ptr<GBuffer> pGBuffer)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		//m_GBuffer = pGBuffer;

		// Set RTVs
		m_bufferRTVs[0] = m_GBuffer->pLightRTV.Get();
		m_bufferRTVs[1] = m_GBuffer->pUUIDRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;
	}
}