#include "Graphics/Renderer/RenderingSystem.h"
#include <Graphics/Renderer/RenderGroup.h>

#include <Utils/DebugUtils.h>

#include <iostream>

namespace SE_G {
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> RenderingSystem::gAnn;

	void RenderingSystem::InitAnnotations(ID3D11DeviceContext* ctx)
	{
		ctx->QueryInterface(__uuidof(ID3DUserDefinedAnnotation),
			reinterpret_cast<void**>(SE_G::RenderingSystem::gAnn.GetAddressOf()));
	}

	RenderingSystem::RenderingSystem()
	{
	}
	
	RenderingSystem::~RenderingSystem()
	{
		/*
		// delete owned RenderGroup pointers before clearing the container
		for (auto &kv : m_renderGroups) {
			if (kv.second) {
				delete kv.second;
				kv.second = nullptr;
			}
		}
		*/
		m_renderGroups.clear();
		m_renderGroupsOrder.clear();

		m_device.Reset();
		m_context.Reset();
		m_swapChain.Reset();
		m_backBuffer.Reset();
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
			m_swapChain.GetAddressOf(),
			m_device.GetAddressOf(),
			nullptr,
			m_context.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Device with Swap Chain");

		// Send to Main RenderPass
		// backBuffer
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBuffer.GetAddressOf()));
		if (FAILED(hr))
			throw std::runtime_error("Failed to get back buffer");

		SE_G::RenderingSystem::InitAnnotations(m_context.Get());
	}

	void RenderingSystem::Render()
	{
		// Groups
		for (size_t i = 0; i < m_renderGroupsOrder.size(); i++)
		{
			const eastl::string &name = m_renderGroupsOrder[i];
			auto it = m_renderGroups.find(name);
			if (it == m_renderGroups.end() || it->second == nullptr) continue;
			if (!it->second->IsEnabled()) continue;
			m_context->ClearState();
			it->second->Pass();
		}
		// PresentFrame();
	}

	void RenderingSystem::PresentFrame() {
		m_swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
		return;
	};

	RenderGroup* RenderingSystem::AddRenderGroup(RenderGroup* renderGroup)
	{
		// to-do: string memory leak
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
		auto it = m_renderGroups.find(groupName);
		if (it != m_renderGroups.end()) {
			if (it->second) {
				// delete it->second;
			}
			m_renderGroups.erase(it);
		}
		for (size_t i = 0; i < m_renderGroupsOrder.size(); i++)
		{
			if (m_renderGroupsOrder[i] == groupName) {
				m_renderGroupsOrder.erase(m_renderGroupsOrder.begin() + i);
				return;
			}
		}
		// log << "RenderGroup not found in RenderingSystem";
		printSunshineMessage(groupName + " RenderGroup not found in RenderingSystem");
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


		m_backBuffer.Reset();
		m_swapChain->ResizeBuffers(
			2,
			m_screenWidth, m_screenHeight,
			m_BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		);

		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBuffer.GetAddressOf()));
		if (FAILED(hr))
			throw std::runtime_error("Failed to get back buffer");
	}
}