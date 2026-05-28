#pragma once

#include <DDSTextureLoader.h>
#include <assimp/material.h>

#include <Graphics/Utils/Color.h>
#include <Graphics/Bindable/Bindable.h>
#include <ResourceManager/IResource.h>

#include <Utils/AssetPath.h>

#include <EASTL/string.h>


namespace SE_G {
	namespace Bind
	{
		class Texture :
			public Bindable, public IResource
		{
		public:
			static SE_G::Color GetColorFromPath(eastl::wstring path);
			static SE_G::Color GetRGBAColorFromPath(eastl::wstring path);
			static eastl::wstring ColorToPath(SE_G::Color col);

			Texture(ID3D11Device* device, AssetPath texturePath, UINT slot = 0u,
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

			void ClearTexture();

			void Release() {
				ClearTexture();
			};

			~Texture() override {
				Release();
			};

			void Bind(ID3D11DeviceContext* context) noexcept override;
			void Bind(ID3D11DeviceContext* context, UINT slot) noexcept override;
			void Unbind(ID3D11DeviceContext* context) noexcept override;

			void UpdateTextureView(ID3D11ShaderResourceView* pTextureView);

			AssetPath m_texturePath;
			AssetPath GetCurrentTexturePath();
			SE_G::Color GetCurrentColor();

			// Inherited via IResource
			SunshineResource::ResourceType GetType() const override;
			ResourceGUID GetGUID() const override;
			size_t GetSizeInMemory() const override;

			void SetSlot(UINT slot);
			UINT GetSlot();
		private:
			void ChangeTexture(ID3D11Device* device, AssetPath texturePath);
			void ChangeColor(ID3D11Device* device, SE_G::Color color);

			bool HasAlpha() const noexcept;
			void Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData);
			void InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height);

			ResourceGUID m_GUID = 0;
			size_t m_MemorySize = 0;

			bool isNull = true;

			UINT m_slot;
			// eastl::wstring m_filePath;

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