#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <EASTL/shared_ptr.h>

#include <stdexcept>

namespace SE_G {
	class GBuffer
	{
	public:
		UINT m_screenWidth = 800u;
		UINT m_screenHeight = 800u;

		// Depth
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthDSV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pDepthSRV;

		// World Position
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pWorldPosBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pWorldPosRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pWorldPosSRV;

		// Normal
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pNormalBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pNormalRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pNormalSRV;

		// Albedo
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pAlbedoBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pAlbedoRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pAlbedoSRV;

		// Specular (Intensity; Power)
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pSpecularBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pSpecularRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSpecularSRV;

		// UUID
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pUUIDBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pUUIDRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pUUIDSRV;

		// --- Light ---
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pLightBuffer;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pLightRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pLightSRV;

		GBuffer(ID3D11Device* device, UINT screenWidth, UINT screenHeight);

		void OnResize(ID3D11Device* device, UINT screenWidth, UINT screenHeight);
	};
}
