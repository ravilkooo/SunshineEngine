#include <Graphics/Renderer/Pass/LightPass.h>


namespace SE_G {
	LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("LightPass", device, context)
	{
		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Light;

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;

		m_camPCB = eastl::make_unique<Bind::PixelConstantBuffer<CamPCB>>(
			device,
			CamPCB{ DX::XMMatrixIdentity(), DX::XMMatrixIdentity(),
			(DX::XMFLOAT3)camera->GetPosition(), 0 },
			0u);
		AddPerFrameBind(m_camPCB.get());

		m_screenInfoPCB = eastl::make_unique<Bind::PixelConstantBuffer<ScreenInfoPCB>>(
			device,
			ScreenInfoPCB{ DXSM::Vector2(
				static_cast<float>(m_screenWidth),
				static_cast<float>(m_screenHeight)) },
			1u);
		AddPerFrameBind(m_screenInfoPCB.get());

		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_defaultBlendState = eastl::make_unique<Bind::BlendState>(device, blendDesc);
		AddPerFrameBind(m_defaultBlendState.get());

		m_NormalTexture = eastl::make_shared<Bind::Texture>(device, pGBuffer->pNormalSRV.Get(), 0u);
		m_AlbedoTexture = eastl::make_shared<Bind::Texture>(device, pGBuffer->pAlbedoSRV.Get(), 1u);
		m_SpecularTexture = eastl::make_shared<Bind::Texture>(device, pGBuffer->pSpecularSRV.Get(), 2u);
		m_WorldPosTexture = eastl::make_shared<Bind::Texture>(device, pGBuffer->pWorldPosSRV.Get(), 3u);

		AddPerFrameBind(m_NormalTexture.get());
		AddPerFrameBind(m_AlbedoTexture.get());
		AddPerFrameBind(m_SpecularTexture.get());
		AddPerFrameBind(m_WorldPosTexture.get());

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

	}

	LightPass::~LightPass()
	{
		particleSystems.clear();
	}

	void LightPass::StartFrame()
	{
		context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
		float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		context->ClearRenderTargetView(m_GBuffer->pLightRTV.Get(), colorBlack);
		//context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
		context->RSSetViewports(1, &m_viewport);

		DX::XMFLOAT3 camPos = m_camera->GetPosition();
		DX::XMMATRIX viewMat = m_camera->GetViewMatrix();
			//DX::XMMatrixTranspose(DX::XMMatrixInverse(nullptr, m_camera->GetViewMatrix()));
		DX::XMMATRIX pMatInverse = DX::XMMatrixTranspose(DX::XMMatrixInverse(nullptr,
			m_camera->GetProjectionMatrix()));
		// camera->GetProjectionMatrix()
		m_camPCB->Update(GetDeviceContext(), { viewMat, pMatInverse, camPos, 0 });
		m_screenInfoPCB->Update(GetDeviceContext(), { DXSM::Vector2(
				static_cast<float>(m_screenWidth),
				static_cast<float>(m_screenHeight)) });

		m_camera->UpdateBuffer(context.Get());
		m_camera->BindBuffer(context.Get());
	}

	void LightPass::Pass()
	{
		BindAllPerFrame();

		/*
		for (const auto& gameObjectUUID : scene.gameObjects) {
			const auto& gameObject = scene.GetGameObjectByUUID(gameObjectUUID);
			if (gameObject->HasComponent<RenderComponent>() &&
				gameObject->HasComponent<TransformComponent>()) {

				auto renderComponent = gameObject->GetComponent<RenderComponent>();

				if (!renderComponent->HasTechnique(techniqueTag))
					continue;

				gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());
				renderComponent->PassTechnique(techniqueTag, GetDeviceContext());

			}
		}
		*/
		for (auto& tech : m_techniques) {
			tech->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech->Pass(GetDeviceContext());
		}

		ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
		context->PSSetShaderResources(0, 4, nullSRVs);

		for (auto ps : particleSystems) {
			ps->Render();
		}

	}

	void LightPass::EndFrame()
	{
		//ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };

		//context->PSSetShaderResources(0, NULL, NULL);
		context->OMSetRenderTargets(0, NULL, NULL);
	}

	eastl::shared_ptr<Camera> LightPass::GetCamera()
	{
		return m_camera;
	}

	void LightPass::SetCamera(eastl::shared_ptr<Camera> camera)
	{
		this->m_camera = camera;
	}

	void LightPass::OnResize(UINT resizeWidth, UINT resizeHeight)
		//eastl::shared_ptr<GBuffer> pGBuffer)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		//m_GBuffer = pGBuffer;

		m_NormalTexture->UpdateTextureView(m_GBuffer->pNormalSRV.Get());
		m_AlbedoTexture->UpdateTextureView(m_GBuffer->pAlbedoSRV.Get());
		m_SpecularTexture->UpdateTextureView(m_GBuffer->pSpecularSRV.Get());
		m_WorldPosTexture->UpdateTextureView(m_GBuffer->pWorldPosSRV.Get());

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