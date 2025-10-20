#include "Graphics/DeferredRenderer.h"

DeferredRenderer::DeferredRenderer()
{
}

DeferredRenderer::DeferredRenderer(HWND hWnd,
	UINT screenWidth, UINT screenHeight)
{
	LPCWSTR applicationName = L"SunshineEngine";
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	// swapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = m_screenWidth;
	swapChainDesc.BufferDesc.Height = m_screenHeight;
	swapChainDesc.BufferDesc.Format = m_BackBufferFormat;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		nullptr,
		&context);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Device with Swap Chain");

	// Send to Main RenderPass
	// backBuffer
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer");

}

void DeferredRenderer::InitGBuffer(UINT screenWidth, UINT screenHeight)
{
	this->pGBuffer = eastl::make_shared<GBuffer>(device.Get(), screenWidth, screenHeight);
	mainCamera = eastl::make_shared<Camera>(device.Get(), screenWidth * 1.0f / screenHeight);
	mainCamera->SetPosition({ 0, 0, -10 });
}

void DeferredRenderer::RenderScene(const Scene& scene)
{
	// Passes
	for (auto pass : passes) {
		context->ClearState();
		pass->StartFrame();
		pass->Pass(scene);
		pass->EndFrame();
	}

	//swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void DeferredRenderer::AddPass(eastl::shared_ptr<RenderPass> pass)
{
	passes.push_back(pass);
}

void DeferredRenderer::PreResize()
{
	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);
}

void DeferredRenderer::OnResize(UINT resizeWidth, UINT resizeHeight)
{
	m_screenWidth = resizeWidth;
	m_screenHeight = resizeHeight;
	

	backBuffer.ReleaseAndGetAddressOf();
	swapChain->ResizeBuffers(
		2,
		m_screenWidth, m_screenHeight,
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);

	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer");
}
