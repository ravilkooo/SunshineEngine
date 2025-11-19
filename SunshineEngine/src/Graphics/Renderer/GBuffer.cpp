#include "Graphics/Renderer/GBuffer.h"

namespace SE_G {
	GBuffer::GBuffer(ID3D11Device* device,
		UINT screenWidth, UINT screenHeight) :
		m_screenWidth(screenWidth), m_screenHeight(screenHeight)
	{
		OnResize(device, screenWidth, screenHeight);
	}

	GBuffer::~GBuffer() {
		Release();
	}

	void GBuffer::OnResize(ID3D11Device* device, UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		Release();

		// Depth Texture
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = m_screenWidth;
		depthDesc.Height = m_screenHeight;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		depthDesc.MiscFlags = 0;
		device->CreateTexture2D(&depthDesc, nullptr, pDepthBuffer.GetAddressOf());
		// Depth DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		device->CreateDepthStencilView(pDepthBuffer.Get(), &descDSV, pDepthDSV.GetAddressOf());
		// Depth DSV
		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
		descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pDepthBuffer.Get(), &descSRV, pDepthSRV.GetAddressOf());

		// WorldPos Texture
		D3D11_TEXTURE2D_DESC worldPosDesc = {};
		worldPosDesc.Width = m_screenWidth;
		worldPosDesc.Height = m_screenHeight;
		worldPosDesc.MipLevels = 1;
		worldPosDesc.ArraySize = 1;
		worldPosDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // World Position
		worldPosDesc.SampleDesc.Count = 1;
		worldPosDesc.Usage = D3D11_USAGE_DEFAULT;
		worldPosDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&worldPosDesc, nullptr, pWorldPosBuffer.GetAddressOf());
		// WorldPos SRV
		descSRV = {};
		descSRV.Format = worldPosDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pWorldPosBuffer.Get(), &descSRV, pWorldPosSRV.GetAddressOf());
		// WorldPos RTV
		HRESULT hr = device->CreateRenderTargetView(pWorldPosBuffer.Get(), nullptr, pWorldPosRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");


		// Normal Texture
		D3D11_TEXTURE2D_DESC normalDesc = {};
		normalDesc.Width = m_screenWidth;
		normalDesc.Height = m_screenHeight;
		normalDesc.MipLevels = 1;
		normalDesc.ArraySize = 1;
		normalDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // Normals
		normalDesc.SampleDesc.Count = 1;
		normalDesc.Usage = D3D11_USAGE_DEFAULT;
		normalDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&normalDesc, nullptr, pNormalBuffer.GetAddressOf());
		// Normal SRV
		descSRV = {};
		descSRV.Format = normalDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pNormalBuffer.Get(), &descSRV, pNormalSRV.GetAddressOf());
		// Normal RTV
		hr = device->CreateRenderTargetView(pNormalBuffer.Get(), nullptr, pNormalRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");

		// Albedo Texture
		D3D11_TEXTURE2D_DESC albedoDesc = {};
		albedoDesc.Width = m_screenWidth;
		albedoDesc.Height = m_screenHeight;
		albedoDesc.MipLevels = 1;
		albedoDesc.ArraySize = 1;
		albedoDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Albedo
		albedoDesc.SampleDesc.Count = 1;
		albedoDesc.Usage = D3D11_USAGE_DEFAULT;
		albedoDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&albedoDesc, nullptr, pAlbedoBuffer.GetAddressOf());
		// Albedo SRV
		descSRV = {};
		descSRV.Format = albedoDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pAlbedoBuffer.Get(), &descSRV, pAlbedoSRV.GetAddressOf());
		// Albedo RTV
		hr = device->CreateRenderTargetView(pAlbedoBuffer.Get(), nullptr, pAlbedoRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");

		// Specular
		D3D11_TEXTURE2D_DESC specularDesc = {};
		specularDesc.Width = m_screenWidth;
		specularDesc.Height = m_screenHeight;
		specularDesc.MipLevels = 1;
		specularDesc.ArraySize = 1;
		specularDesc.Format = DXGI_FORMAT_R16G16_FLOAT; // Specular (Intensity; Power)
		specularDesc.SampleDesc.Count = 1;
		specularDesc.Usage = D3D11_USAGE_DEFAULT;
		specularDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&specularDesc, nullptr, pSpecularBuffer.GetAddressOf());
		// Specular SRV
		descSRV = {};
		descSRV.Format = specularDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pSpecularBuffer.Get(), &descSRV, pSpecularSRV.GetAddressOf());
		// Specular RTV
		hr = device->CreateRenderTargetView(pSpecularBuffer.Get(), nullptr, pSpecularRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");

		// UUID Texture
		D3D11_TEXTURE2D_DESC UUIDDesc = {};
		UUIDDesc.Width = m_screenWidth;
		UUIDDesc.Height = m_screenHeight;
		UUIDDesc.MipLevels = 1;
		UUIDDesc.ArraySize = 1;
		UUIDDesc.Format = DXGI_FORMAT_R32G32_UINT; // UUID
		UUIDDesc.SampleDesc.Count = 1;
		UUIDDesc.Usage = D3D11_USAGE_DEFAULT;
		UUIDDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&UUIDDesc, nullptr, pUUIDBuffer.GetAddressOf());
		// UUID SRV
		descSRV = {};
		descSRV.Format = UUIDDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pUUIDBuffer.Get(), &descSRV, pUUIDSRV.GetAddressOf());
		// UUID RTV
		hr = device->CreateRenderTargetView(pUUIDBuffer.Get(), nullptr, pUUIDRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");

		// LightPass stuff

		D3D11_TEXTURE2D_DESC lightDesc = {};
		lightDesc.Width = m_screenWidth;
		lightDesc.Height = m_screenHeight;
		lightDesc.MipLevels = 1;
		lightDesc.ArraySize = 1;
		lightDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		lightDesc.SampleDesc.Count = 1;
		lightDesc.Usage = D3D11_USAGE_DEFAULT;
		lightDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		device->CreateTexture2D(&lightDesc, nullptr, pLightBuffer.GetAddressOf());
		// Light SRV
		descSRV = {};
		descSRV.Format = lightDesc.Format;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2DArray.MostDetailedMip = 0;
		descSRV.Texture2DArray.MipLevels = 1;
		descSRV.Texture2DArray.FirstArraySlice = 0;
		descSRV.Texture2DArray.ArraySize = 1;
		device->CreateShaderResourceView(pLightBuffer.Get(), &descSRV, pLightSRV.GetAddressOf());
		// Light RTV
		hr = device->CreateRenderTargetView(pLightBuffer.Get(), nullptr, pLightRTV.GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Failed to create Render Target View");
	}

	void GBuffer::Release() {
		pDepthBuffer.Reset();
		pDepthDSV.Reset();
		pDepthSRV.Reset();

		pWorldPosBuffer.Reset();
		pWorldPosRTV.Reset();
		pWorldPosSRV.Reset();

		pNormalBuffer.Reset();
		pNormalRTV.Reset();
		pNormalSRV.Reset();

		pAlbedoBuffer.Reset();
		pAlbedoRTV.Reset();
		pAlbedoSRV.Reset();

		pSpecularBuffer.Reset();
		pSpecularRTV.Reset();
		pSpecularSRV.Reset();

		pUUIDBuffer.Reset();
		pUUIDRTV.Reset();
		pUUIDSRV.Reset();

		pLightBuffer.Reset();
		pLightRTV.Reset();
		pLightSRV.Reset();
	}
}