#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <wrl.h>
#include <wrl/client.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace SE_G {
	class RenderGroup;

	class RenderingSystem
	{
	public:
		// temp
		static Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> gAnn;

		static void InitAnnotations(ID3D11DeviceContext* ctx);

		RenderingSystem();
		~RenderingSystem();
		RenderingSystem(HWND hWnd, UINT screenWidth, UINT screenHeight);

		ID3D11Device* GetDevice() { return m_device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return m_context.Get(); }

		void Render();
		void PresentFrame();

		RenderGroup* AddRenderGroup(RenderGroup* renderGroup);
		void RemoveRenderGroup(eastl::string groupName);

		ID3D11Texture2D* GetBackBuffer() { return m_backBuffer.Get(); }
		void PreResize();
		void OnResize(UINT resizeWidth, UINT resizeHeight);

	protected:
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;

		DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_1 };

		UINT m_screenWidth = 800u;
		UINT m_screenHeight = 800u;

		eastl::vector<eastl::string> m_renderGroupsOrder;
		eastl::unordered_map<
			eastl::string, RenderGroup*>
			m_renderGroups;
	};
}
