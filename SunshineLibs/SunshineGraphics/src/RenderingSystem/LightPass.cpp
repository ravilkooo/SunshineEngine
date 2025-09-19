#include "RenderingSystem/LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera)
	:
	RenderPass("LightPass", device, context)
{
	this->pGBuffer = pGBuffer;
	this->camera = camera;

	D3D11_TEXTURE2D_DESC lightDesc = {};
	lightDesc.Width = screenWidth;
	lightDesc.Height = screenHeight;
	lightDesc.MipLevels = 1;
	lightDesc.ArraySize = 1;
	lightDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	lightDesc.SampleDesc.Count = 1;
	lightDesc.Usage = D3D11_USAGE_DEFAULT;
	lightDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&lightDesc, nullptr, pGBuffer->pLightBuffer.GetAddressOf());
	// Normal SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
	descSRV.Format = lightDesc.Format;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2DArray.MostDetailedMip = 0;
	descSRV.Texture2DArray.MipLevels = 1;
	descSRV.Texture2DArray.FirstArraySlice = 0;
	descSRV.Texture2DArray.ArraySize = 1;
	device->CreateShaderResourceView(pGBuffer->pLightBuffer.Get(), &descSRV, pGBuffer->pLightSRV.GetAddressOf());
	// Normal RTV
	HRESULT hr = device->CreateRenderTargetView(pGBuffer->pLightBuffer.Get(), nullptr, pGBuffer->pLightRTV.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

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
}

void LightPass::Pass(const Scene& scene)
{
	BindAllPerFrame();

	for (SceneNode* node : scene.nodes) {
		if (!node->HasTechnique(techniqueTag))
			continue;
		// Turn off depth write
		// Prepare Lights (depthstencil desc and rast desc) and bind (depthstencil and rast)
		LightObject* lObj = dynamic_cast<LightObject*>(node);
		if (lObj) {
			lObj->UpdateBuffers(context);
			LightObject::LightPosition lightPos = lObj->GetLightPositionInFrustum(GetCamera());

			/*
			if (dynamic_cast<PointLight*>(node)) {
				if (lightPos == LightObject::LightPosition::BEHIND_NEAR_PLANE
					|| lightPos == LightObject::LightPosition::INSIDE
					|| lightPos == LightObject::LightPosition::INTERSECT_FAR_PLANE
					|| lightPos == LightObject::LightPosition::FILL)
				*/
			auto dsDesc = lObj->GetDepthStencilDesc(lightPos);
			auto rastDesc = lObj->GetRasterizerDesc(lightPos);

			ID3D11RasterizerState* rasterState;
			ID3D11DepthStencilState* depthState;
			device->CreateRasterizerState(&rastDesc, &rasterState);
			device->CreateDepthStencilState(&dsDesc, &depthState);

			context->OMSetDepthStencilState(depthState, 0);
			context->RSSetState(rasterState);

			node->PassTechnique(techniqueTag, GetDeviceContext());

			context->OMSetDepthStencilState(nullptr, 0);
			context->RSSetState(nullptr);

		}
		else {
			continue;
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

Camera* LightPass::GetCamera()
{
	return camera;
}

void LightPass::SetCamera(Camera* camera)
{
	this->camera = camera;
}
