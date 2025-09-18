

#include "TextureB.h"
#include <iostream>

namespace Bind
{
	TextureB::TextureB(ID3D11Device* device, ID3D11Resource* pTexture,
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc, UINT slot, Bind::PipelineStage pipelineStage)
		: pTexture(pTexture), slot(slot), pipelineStage(pipelineStage)
	{
		auto hr = device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create shader resource view from texture generated from color data.");
		}
	}
	TextureB::TextureB(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), pipelineStage(pipelineStage)
	{
		this->pTexture = pTexture;
		this->pTextureView = pTextureView;
	}
	TextureB::TextureB(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), pipelineStage(pipelineStage)
	{
		this->pTexture = nullptr;
		this->pTextureView = pTextureView;
	}
	TextureB::TextureB(ID3D11Device* device, const std::string& filePath, aiTextureType type, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), filePath(filePath), type(type), pipelineStage(pipelineStage)
	{
		if (StringHelper::GetFileExtension(filePath) == "dds")
		{
			//std::cout << "DDS loaded!!! " << filePath << " :: " << StringHelper::GetFileExtension(filePath) << "\n";
			HRESULT hr = DirectX::CreateDDSTextureFromFile(device,
				StringHelper::StringToWide(filePath).c_str(), &pTexture, &pTextureView);
			if (FAILED(hr))
			{
				this->Initialize1x1ColorTexture(device, SE_Colors::UnloadedTextureColor, type);
			}
			return;
		}
		else
		{
			std::cout << "Wrong texture file extension: " << StringHelper::GetFileExtension(filePath) << "\n";
			this->Initialize1x1ColorTexture(device, SE_Colors::UnloadedTextureColor, type);
			/*
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), *pTexture, GetTextureResourceViewAddress());
			if (FAILED(hr))
			{
				this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
			}
			return;
			*/
		}
	}

	TextureB::TextureB(ID3D11Device* device, const SE_Color& color, aiTextureType type, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), pipelineStage(pipelineStage)
	{
		this->Initialize1x1ColorTexture(device, color, type);
	}

	TextureB::TextureB(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type, UINT slot, Bind::PipelineStage pipelineStage)
		: slot(slot), pipelineStage(pipelineStage)
	{
		this->InitializeColorTexture(device, colorData, width, height, type);
	}

	void TextureB::Bind(ID3D11DeviceContext* context) noexcept
	{
		if (pipelineStage == Bind::PipelineStage::PIXEL_SHADER)
			context->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		else if (pipelineStage == Bind::PipelineStage::COMPUTE_SHADER)
			context->CSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
		//context->PSSetShaderResources(0, 1u, pTextureView.GetAddressOf());
	}

	void TextureB::Unbind(ID3D11DeviceContext* context) noexcept
	{
		ID3D11ShaderResourceView* nullSRVs[] = {nullptr};
		if (pipelineStage == Bind::PipelineStage::PIXEL_SHADER)
			context->PSSetShaderResources(slot, 1u, nullSRVs);
		else if (pipelineStage == Bind::PipelineStage::COMPUTE_SHADER)
			context->CSSetShaderResources(slot, 1u, nullSRVs);
	}

	bool TextureB::HasAlpha() const noexcept
	{
		return hasAlpha;
	}

	aiTextureType TextureB::GetType()
	{
		return this->type;
	}

	void TextureB::Initialize1x1ColorTexture(ID3D11Device* device, const SE_Color& colorData, aiTextureType type)
	{
		InitializeColorTexture(device, &colorData, 1, 1, type);
	}

	void TextureB::InitializeColorTexture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type)
	{
		this->type = type;

		/*D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		*/
		CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

		D3D11_SUBRESOURCE_DATA initialData{};
		initialData.pSysMem = colorData;
		initialData.SysMemPitch = width * sizeof(SE_Color);

		ID3D11Texture2D* p2DTexture = nullptr;
		HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to initialize texture from color data.");
		}

		pTexture = static_cast<ID3D11Texture2D*>(p2DTexture);
		/*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MipLevels = 1;*/

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

		hr = device->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView);
		if (FAILED(hr)) {
			throw std::runtime_error("Failed to create shader resource view from texture generated from color data.");
		}
	}

}
