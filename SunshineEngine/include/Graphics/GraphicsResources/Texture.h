#pragma once

//#include <IResource.h>

#include <DDSTextureLoader.h>
#include <assimp/material.h>

#include "Utils/StringHelper.h"
#include <Graphics/Utils/Color.h>
#include <Graphics/Bindable/Bindable.h>
#include <Graphics/Bindable/Sampler.h>

#include <EASTL/string.h>


namespace SE_G {
	namespace Bind
	{
		class Texture :
			public Bindable
			// , public Resource
		{
		public:
			Texture(ID3D11Device* device, const eastl::wstring& filePath, UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);

			Texture(ID3D11Device* device, ID3D11Resource* pTexture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc,
				UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
			Texture(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
			Texture(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);

			Texture(ID3D11Device* device, const Color& color, UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
			Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, UINT slot = 0u,
				Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);

			void ChangeTexture(ID3D11Device* device, const eastl::wstring& filePath);
			void ChangeColor(ID3D11Device* device, SE_G::Color color);

			void ClearTexture();

			void Release() {
				ClearTexture();
			};

			~Texture() override {
				Release();
			};

			void Bind(ID3D11DeviceContext* context) noexcept override;
			void Unbind(ID3D11DeviceContext* context) noexcept override;

			void UpdateTextureView(ID3D11ShaderResourceView* pTextureView);

			bool HasAlpha() const noexcept;
			void Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData);
			void InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height);

			eastl::wstring GetCurrentTexturePath();
			SE_G::Color GetCurrentColor();

		private:
			bool isNull = true;

			UINT m_slot;
			eastl::wstring m_filePath;
			bool hasAlpha = false;
			Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;

			// otherwise textured
			bool m_colored = true;
			SE_G::Color m_color = SE_G::Colors::UnloadedTextureColor;
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER;
		};

	}
}