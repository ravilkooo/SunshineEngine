#pragma once



#include "Bindable.h"
#include "GraphicsUtils/SE_Color.h"
#include "Utils/StringHelper.h"
#include <DDSTextureLoader.h>
#include <assimp/material.h>

namespace Bind
{
	class TextureB :
		public Bindable
	{
	public:
		TextureB(ID3D11Device* device, ID3D11Resource* pTexture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc,
			UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		TextureB(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		TextureB(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		TextureB(ID3D11Device* device, const std::string& filePath, aiTextureType type, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		TextureB(ID3D11Device* device, const SE_Color& color, aiTextureType type, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		TextureB(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		void Bind(ID3D11DeviceContext* context) noexcept override;
		void Unbind(ID3D11DeviceContext* context) noexcept override;
		bool HasAlpha() const noexcept;
		aiTextureType GetType();
	private:
		void Initialize1x1ColorTexture(ID3D11Device* device, const SE_Color& colorData, aiTextureType type);
		void InitializeColorTexture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type);

		UINT slot;
		std::string filePath;
		bool hasAlpha = false;
		Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;

		Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER;
		//unsigned int slot;
	};

}
