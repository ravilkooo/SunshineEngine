#pragma once

#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>

#include <EASTL/shared_ptr.h>

#include "Scene.h"
#include "Graphics/Renderer/Pass/RenderPass.h"
#include <Graphics/Utils/Camera.h>

namespace SE_G {
	class RenderingSystem
	{
	public:
		ID3D11Device* GetDevice() { return device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return context.Get(); }

		virtual void RenderScene() = 0;
		virtual void PresentFrame() {
			swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
			return;
		};

		virtual void SetMainCamera(eastl::shared_ptr<Camera> camera) { this->mainCamera = camera; }
		virtual eastl::shared_ptr<Camera> GetMainCamera() { return mainCamera; };

		ID3D11Texture2D* GetBackBuffer() { return backBuffer.Get(); }

		virtual void AddPass(eastl::shared_ptr<RenderPass> pass) = 0;

		eastl::vector<eastl::shared_ptr<RenderPass>> m_passes;
		eastl::shared_ptr<Camera> mainCamera;
	protected:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

		D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

		UINT m_screenWidth = 800;
		UINT m_screenHeight = 800;

	};
}
