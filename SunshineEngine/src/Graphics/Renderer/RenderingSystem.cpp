#include "Graphics/Renderer/RenderingSystem.h"
#include <iostream>

namespace SE_G {
	RenderingSystem::RenderingSystem()
	{
	}
	
	RenderingSystem::~RenderingSystem()
	{
		m_renderGroupsOrder.clear();
		m_renderGroups.clear();

		m_device.ReleaseAndGetAddressOf();
		m_context.ReleaseAndGetAddressOf();
		m_swapChain.ReleaseAndGetAddressOf();
		m_backBuffer.ReleaseAndGetAddressOf();
	}

	RenderingSystem::RenderingSystem(HWND hWnd,
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
			&m_swapChain,
			&m_device,
			nullptr,
			&m_context);
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Device with Swap Chain");

		// Send to Main RenderPass
		// backBuffer
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);
		if (FAILED(hr))
			throw std::runtime_error("Failed to get back buffer");

	}

	void RenderingSystem::Render()
	{
		// Groups
		for (size_t i = 0; i < m_renderGroupsOrder.size(); i++)
		{
			if (!m_renderGroups[m_renderGroupsOrder[i]]->IsEnabled())
				continue;
			m_context->ClearState();
			m_renderGroups[m_renderGroupsOrder[i]]->Pass();
		}
		// PresentFrame();
	}

	void RenderingSystem::PresentFrame() {
		m_swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
		return;
	};

	RenderGroup* RenderingSystem::AddRenderGroup(RenderGroup* renderGroup)
	{
		eastl::string name = renderGroup->m_groupName;
		auto [it, inserted] = m_renderGroups.emplace(name, nullptr);
		if (!inserted)
		{
			// log << "Duplicate RenderGroup in RenderingSystem::AddRenderGroup!";
			printf("Duplicate RenderGroup in RenderingSystem::AddRenderGroup!\n");
			return nullptr;
		}
		it->second = renderGroup;
		m_renderGroupsOrder.push_back(it->second->m_groupName);
		return it->second;
		/*
		if (m_renderGroups.contains(renderGroup->m_groupName)) {
			// log << contains;
			printf("Duplicate RenderGroup in RenderingSystem::AddRenderGroup!");
			return;
		}
		m_renderGroups[renderGroup->m_groupName] = renderGroup;
		m_renderGroupsOrder.push_back(renderGroup->m_groupName);
		return;
		*/
		/*
		eastl::string name = renderGroup->m_groupName;
		auto [it, inserted] = m_renderGroups.emplace(name, nullptr);
		if (!inserted)
		{
			// log << "Duplicate RenderGroup in RenderingSystem::AddRenderGroup!";
			printf("Duplicate RenderGroup in RenderingSystem::AddRenderGroup!\n");
			return nullptr;
		}
		it->second = eastl::move(renderGroup);
		m_renderGroupsOrder.push_back(it->second->m_groupName);
		return it->second.get();
		*/
	}
	
	void RenderingSystem::RemoveRenderGroup(eastl::string groupName)
	{
		m_renderGroups.erase(groupName);
		for (size_t i = 0; i < m_renderGroupsOrder.size(); i++)
		{
			if (m_renderGroupsOrder[i] == groupName) {
				m_renderGroupsOrder.erase(m_renderGroupsOrder.begin() + i);
				return;
			}
		}
		// log << "RenderGroup not found in RenderingSystem";
		printf("RenderGroup not found in RenderingSystem\n");
	}

	void RenderingSystem::PreResize()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr };
		m_context->OMSetRenderTargets(1, nullRTVs, nullptr);
	}

	void RenderingSystem::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;


		m_backBuffer.ReleaseAndGetAddressOf();
		m_swapChain->ResizeBuffers(
			2,
			m_screenWidth, m_screenHeight,
			m_BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		);

		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);
		if (FAILED(hr))
			throw std::runtime_error("Failed to get back buffer");
	}
}