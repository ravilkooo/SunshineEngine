

#include "MainColorPass.h"

MainColorPass::MainColorPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight)
	:
	RenderPass("MainColorPass", device, context)
{
	this->backBuffer = backBuffer;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// rtv
	HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// depth buffer
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	camera = new Camera(screenWidth * 1.0f / screenHeight);
}

void MainColorPass::StartFrame()
{
	context->OMSetRenderTargets(1u, &renderTargetView, pDSV);
	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, color);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);
}

Camera* MainColorPass::GetCamera()
{
	return camera;
}

void MainColorPass::SetCamera(Camera* camera)
{
	this->camera = camera;
}

void MainColorPass::EndFrame()
{
	ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRVs);
	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);
	//context->OMSetRenderTargets(0, NULL, NULL);
}
