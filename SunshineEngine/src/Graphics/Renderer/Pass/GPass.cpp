#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/GBuffer.h>
#include "Graphics/Renderer/Pass/GPass.h"
#include <Graphics/Renderer/Technique/RenderTechnique.h>

#include <Graphics/Utils/Camera.h>

#include <Component/TransformComponent.h>

namespace SE_G {
	GPass::GPass(DeferredRenderer* renderer,
		eastl::shared_ptr<GBuffer> pGBuffer)
		:
		RenderPass("GPass", renderer)
	{
		this->screenWidth = pGBuffer->m_screenWidth;
		this->screenHeight = pGBuffer->m_screenHeight;
		this->pGBuffer = pGBuffer;
		m_passType = PassType::GPass;

		// Set RTVs
		gBufferRTVs[0] = pGBuffer->pNormalRTV.Get();
		gBufferRTVs[1] = pGBuffer->pAlbedoRTV.Get();
		gBufferRTVs[2] = pGBuffer->pSpecularRTV.Get();
		gBufferRTVs[3] = pGBuffer->pWorldPosRTV.Get();
		gBufferRTVs[4] = pGBuffer->pUUIDRTV.Get();

		// Viewport
		viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

	}

	GPass::~GPass()
	{
		//delete[] gBufferRTVs;
	}

	void GPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"GPass");
		auto context = m_renderer->GetDeviceContext();

		context->OMSetRenderTargets(5, gBufferRTVs, pGBuffer->pDepthDSV.Get());
		float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float colorNone[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		// uint32_t UUIDNone[] = { 0u, 0u };
		auto camPos = m_renderer->GetMainCamera()->GetPosition();
		auto camDir = m_renderer->GetMainCamera()->GetForward();
		float farZ = m_renderer->GetMainCamera()->GetFarZ();
		auto farPoint = camPos + camDir * farZ;
		float colorFar[] = { farPoint.x, farPoint.y, farPoint.z, 1.0f };
		context->ClearRenderTargetView(gBufferRTVs[0], colorBlack);
		context->ClearRenderTargetView(gBufferRTVs[1], colorBlack);
		context->ClearRenderTargetView(gBufferRTVs[2], colorNone);
		context->ClearRenderTargetView(gBufferRTVs[3], colorFar);
		context->ClearRenderTargetView(gBufferRTVs[4], colorNone);
		context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
		context->RSSetViewports(1, &viewport);

		// Bind camera buffer to 1u slot
		m_renderer->GetMainCamera()->BindBuffer(context);
	}

	void GPass::Pass()
	{
		BindAllPerFrame();
		for (auto& tech : m_techniques) {
			if (!tech.second->IsEnabled())
				continue;
			tech.second->m_assignedTransform->EnableMeshTransformMode();
			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());
			tech.second->m_assignedTransform->DisableMeshTransformMode();
		}
	}

	void GPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		m_renderer->GetDeviceContext()->OMSetRenderTargets(5, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	// GBuffer should be resized before this method
	void GPass::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		screenWidth = resizeWidth;
		screenHeight = resizeHeight;

		// Set RTVs
		gBufferRTVs[0] = pGBuffer->pNormalRTV.Get();
		gBufferRTVs[1] = pGBuffer->pAlbedoRTV.Get();
		gBufferRTVs[2] = pGBuffer->pSpecularRTV.Get();
		gBufferRTVs[3] = pGBuffer->pWorldPosRTV.Get();
		gBufferRTVs[4] = pGBuffer->pUUIDRTV.Get();

		// Viewport
		viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;
	}
}