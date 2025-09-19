#pragma once



#include <d3d11.h>
#include <wrl.h>

class GBuffer
{
public:
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

	// --- Light ---
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pLightRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pLightSRV;
	

	// Don't need it
	/*
	// Diffuse
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDiffuseLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pDiffuseLightRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pDiffuseLightSRV;
	// Specular
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pSpecularLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pSpecularLightRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSpecularLightSRV;
	// Ambient
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pAmbientLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pAmbientLightRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pAmbientLightSRV;
	*/
};

