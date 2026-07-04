#include <Graphics/Renderer/Pass/LightPass.h>
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/GBuffer.h>

#include <Graphics/Renderer/Technique/RenderTechnique.h>

#include <Graphics/Bindable/Bindable.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/BlendState.h>
#include <Graphics/Bindable/ConstantBuffer.h>

#include <Graphics/GraphicsResources/Texture.h>

#include <Graphics/Utils/Camera.h>

#include <Component/TransformComponent.h>

#include <ParticleSystem/ParticleSystem.h>

namespace SE_G {
	LightPass::LightPass(DeferredRenderer* renderer,
		eastl::shared_ptr<GBuffer> pGBuffer)
		:
		RenderPass("LightPass", renderer)
	{
		this->m_GBuffer = pGBuffer;
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

		auto device = renderer->GetDevice();
		m_camPCB = eastl::make_unique<Bind::PixelConstantBuffer<CamPCB>>(
			device,
			CamPCB{ DX::XMMatrixIdentity(), DX::XMMatrixIdentity(),
			(DX::XMFLOAT3)m_renderer->GetMainCamera()->GetPosition(), 0 },
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
	}

	void LightPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Light Pass");

		auto context = m_renderer->GetDeviceContext();
		context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
		float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		context->ClearRenderTargetView(m_GBuffer->pLightRTV.Get(), colorBlack);
		context->RSSetViewports(1, &m_viewport);

		auto camera = m_renderer->GetMainCamera();
		DX::XMFLOAT3 camPos = camera->GetPosition();
		DX::XMMATRIX viewMat = camera->GetViewMatrix();
		DX::XMMATRIX pMatInverse = DX::XMMatrixTranspose(DX::XMMatrixInverse(nullptr,
			camera->GetProjectionMatrix()));
		// camera->GetProjectionMatrix()
		m_camPCB->Update(GetDeviceContext(), { viewMat, pMatInverse, camPos, 0 });
		m_screenInfoPCB->Update(GetDeviceContext(), { DXSM::Vector2(
				static_cast<float>(m_screenWidth),
				static_cast<float>(m_screenHeight)) });

		BindAllPerFrame();
		camera->UpdateBuffer(context);
		camera->BindBuffer(context);
	}

	void LightPass::Pass()
	{

		for (auto& tech : m_techniques) {
			if (!tech.second->IsEnabled())
				continue;
			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());
		}

		auto context = m_renderer->GetDeviceContext();
		if (m_particleSystem)
		{
			context->ClearState();
			context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
			context->RSSetViewports(1, &m_viewport);
			m_particleSystem->RenderAllEmitters();
		}

		ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
		context->PSSetShaderResources(0, 4, nullSRVs);
	}

	void LightPass::EndFrame()
	{
		m_renderer->GetDeviceContext()->OMSetRenderTargets(0, NULL, NULL);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
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