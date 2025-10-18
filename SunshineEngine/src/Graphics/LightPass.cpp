#include "Graphics/LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, eastl::shared_ptr<GBuffer> pGBuffer, eastl::shared_ptr<Camera> camera)
	:
	RenderPass("LightPass", device, context)
{
	this->pGBuffer = pGBuffer;
	this->camera = camera;

	gBufferRTV = pGBuffer->pLightRTV.Get();

	// Viewport
	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	camPCB = new Bind::PixelConstantBuffer<CamPCB>(device,
		{ XMMatrixIdentity(), XMMatrixIdentity(),
		(XMFLOAT3)camera->GetPosition(), 0 },
		0u);
	AddPerFrameBind(camPCB);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	AddPerFrameBind(new Bind::BlendState(device, blendDesc));


	AddPerFrameBind(new Bind::Texture(device, pGBuffer->pNormalSRV.Get(), 0u));
	AddPerFrameBind(new Bind::Texture(device, pGBuffer->pAlbedoSRV.Get(), 1u));
	AddPerFrameBind(new Bind::Texture(device, pGBuffer->pSpecularSRV.Get(), 2u));
	AddPerFrameBind(new Bind::Texture(device, pGBuffer->pWorldPosSRV.Get(), 3u));

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
	
}

void LightPass::StartFrame()
{
	context->OMSetRenderTargets(1, &gBufferRTV, pGBuffer->pDepthDSV.Get());
	float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(gBufferRTV, colorBlack);
	//context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);

	XMFLOAT3 camPos = camera->GetPosition();
	XMMATRIX vMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetViewMatrix()));
	XMMATRIX pMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetProjectionMatrix()));
	// camera->GetProjectionMatrix()
	camPCB->Update(GetDeviceContext(), { vMatInverse, pMatInverse, camPos, 0 });

	camera->UpdateBuffer(context.Get());
	camera->BindBuffer(context.Get());
}

void LightPass::Pass(const Scene& scene)
{
	BindAllPerFrame();


	for (const auto& gameObject : scene.gameObjects) {
		if (gameObject->HasComponent<RenderComponent>() &&
			gameObject->HasComponent<TransformComponent>()) {

			auto renderComponent = gameObject->GetComponent<RenderComponent>();

			if (!renderComponent->HasTechnique(techniqueTag))
				continue;

			gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());
			renderComponent->PassTechnique(techniqueTag, GetDeviceContext());

		}
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
	return camera;
}

void LightPass::SetCamera(eastl::shared_ptr<Camera> camera)
{
	this->camera = camera;
}
