#include "Graphics/Renderer/Pass/GPass.h"

namespace SE_G {
	GPass::GPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("GPass", device, context)
	{
		this->screenWidth = pGBuffer->m_screenWidth;
		this->screenHeight = pGBuffer->m_screenHeight;
		this->pGBuffer = pGBuffer;
		this->camera = camera;


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

	void GPass::StartFrame()
	{
		context->OMSetRenderTargets(5, gBufferRTVs, pGBuffer->pDepthDSV.Get());
		float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float colorNone[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float colorFar[] = { 10000.0f, 10000.0f, 10000.0f, 1.0f };
		uint32_t UUIDNone[] = { 0u, 0u };
		context->ClearRenderTargetView(gBufferRTVs[0], colorBlack);
		context->ClearRenderTargetView(gBufferRTVs[1], colorBlack);
		context->ClearRenderTargetView(gBufferRTVs[2], colorNone);
		context->ClearRenderTargetView(gBufferRTVs[3], colorFar);
		context->ClearRenderTargetView(gBufferRTVs[4], colorNone);
		context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
		context->RSSetViewports(1, &viewport);

		// Bind camera buffer to 1u slot
		camera->UpdateBuffer(context.Get());
		camera->BindBuffer(context.Get());
	}

	void GPass::Pass(const Scene& scene)
	{
		BindAllPerFrame();

		for (const auto& gameObjectUUID : scene.gameObjects) {
			const auto& gameObject = scene.GetGameObjectByUUID(gameObjectUUID);
			if (gameObject->HasComponent<RenderComponent>() &&
				gameObject->HasComponent<TransformComponent>()) {

				auto renderComponent = gameObject->GetComponent<RenderComponent>();

				if (!renderComponent->HasTechnique(techniqueTag))
					continue;

				gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());
				renderComponent->PassTechnique(techniqueTag, GetDeviceContext()); // Bind + Draw
			}
		}
	}

	eastl::shared_ptr<Camera> GPass::GetCamera()
	{
		return camera;
	}

	void GPass::SetCamera(eastl::shared_ptr<Camera> camera)
	{
		this->camera = camera;
	}

	void GPass::EndFrame()
	{
		//context->PSSetShaderResources(0, NULL, NULL);
		//context->OMSetRenderTargets(0, NULL, NULL);
		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(5, nullRTVs, *nullDSVs);
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