#include "Graphics/GraphicsResources/Texture.h"
#include <iostream>


namespace SE_G {
	namespace Bind
	{
		Texture::Texture(ID3D11Device* device, ID3D11Resource* pTexture,
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc, UINT slot, Bind::PipelineStage pipelineStage)
			: pTexture(pTexture), m_slot(slot), pipelineStage(pipelineStage)
		{
			auto hr = device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);
			if (FAILED(hr)) {
				throw std::runtime_error("Failed to create shader resource view from texture generated from color data.");
			}
			isNull = false;
		}

		Texture::Texture(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView, UINT slot, Bind::PipelineStage pipelineStage)
			: m_slot(slot), pipelineStage(pipelineStage)
		{
			this->pTexture = pTexture;
			this->pTextureView = pTextureView;
			isNull = false;
		}

		Texture::Texture(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView, UINT slot, Bind::PipelineStage pipelineStage)
			: m_slot(slot), pipelineStage(pipelineStage)
		{
			this->pTexture = nullptr;
			this->pTextureView = pTextureView;
			isNull = false;
		}

		Texture::Texture(ID3D11Device* device, const eastl::wstring& filePath, UINT slot, Bind::PipelineStage pipelineStage)
			: m_slot(slot), m_filePath(filePath), pipelineStage(pipelineStage)
		{
			ChangeTexture(device, filePath);
			isNull = false;
		}

		Texture::Texture(ID3D11Device* device, const Color& color, UINT slot, Bind::PipelineStage pipelineStage)
			: m_slot(slot), pipelineStage(pipelineStage)
		{
			this->Initialize1x1ColorTexture(device, color);
			isNull = false;
		}

		Texture::Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, UINT slot, Bind::PipelineStage pipelineStage)
			: m_slot(slot), pipelineStage(pipelineStage)
		{
			this->InitializeColorTexture(device, colorData, width, height);
			isNull = false;
		}

		void Texture::ChangeTexture(ID3D11Device* device, const eastl::wstring& filePath) {
			ClearTexture();
			m_colored = false;
			m_filePath = filePath;

			if (StringHelper::GetFileExtension(filePath) == L"dds")
			{
				//std::cout << "DDS loaded!!! " << filePath << " :: " << StringHelper::GetFileExtension(filePath) << "\n";
				HRESULT hr = DirectX::CreateDDSTextureFromFile(device,
					(filePath).c_str(), &pTexture, &pTextureView);
				if (FAILED(hr))
				{
					this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor);
				}
				return;
			}
			else
			{
				wprintf(L"Wrong texture file extension: %ls\n", StringHelper::GetFileExtension(filePath).c_str());
				this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor);
				/*
				HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), *pTexture, GetTextureResourceViewAddress());
				if (FAILED(hr))
				{
					this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor);
				}
				return;
				*/
			}
		}

		void Texture::ChangeColor(ID3D11Device* device, SE_G::Color color) {
			ClearTexture();
			m_color = color;
			m_colored = true;
			Initialize1x1ColorTexture(device, m_color);
		}

		void Texture::ClearTexture() {
			m_filePath.clear();
			if (!isNull) {
				pTexture.ReleaseAndGetAddressOf();
				pTextureView.ReleaseAndGetAddressOf();
			}
		}

		void Texture::Bind(ID3D11DeviceContext* context) noexcept
		{
			if (pipelineStage == Bind::PipelineStage::PIXEL_SHADER)
				context->PSSetShaderResources(m_slot, 1u, pTextureView.GetAddressOf());
			else if (pipelineStage == Bind::PipelineStage::COMPUTE_SHADER)
				context->CSSetShaderResources(m_slot, 1u, pTextureView.GetAddressOf());
			//context->PSSetShaderResources(0, 1u, pTextureView.GetAddressOf());


		}

		void Texture::Unbind(ID3D11DeviceContext* context) noexcept
		{
			ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
			if (pipelineStage == Bind::PipelineStage::PIXEL_SHADER)
				context->PSSetShaderResources(m_slot, 1u, nullSRVs);
			else if (pipelineStage == Bind::PipelineStage::COMPUTE_SHADER)
				context->CSSetShaderResources(m_slot, 1u, nullSRVs);
		}

		bool Texture::HasAlpha() const noexcept
		{
			return hasAlpha;
		}

		void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData)
		{
			m_color = colorData;
			m_colored = true;
			InitializeColorTexture(device, &colorData, 1, 1);
		}

		void Texture::InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height)
		{

			/*D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			*/
			CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

			D3D11_SUBRESOURCE_DATA initialData{};
			initialData.pSysMem = colorData;
			initialData.SysMemPitch = width * sizeof(Color);

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
		void Texture::UpdateTextureView(ID3D11ShaderResourceView* pTextureView)
		{
			this->pTextureView.ReleaseAndGetAddressOf();
			this->pTextureView = pTextureView;
		}


		eastl::wstring Texture::GetCurrentTexturePath() {
			return m_filePath;
		}

		SE_G::Color Texture::GetCurrentColor() {
			return m_color;
		}
	}
}