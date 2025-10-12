#pragma once

#include <Resource.h>

#include <DDSTextureLoader.h>
#include <assimp/material.h>

#include "GraphicsUtils/SE_Color.h"
#include "Utils/StringHelper.h"
#include "Bindable/Bindable.h"
#include "Bindable/Sampler.h"


namespace Bind
{
	class Texture :
		public Bindable, Resource
	{
	public:
		Texture(ID3D11Device* device, const std::string& filePath, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);

		Texture(ID3D11Device* device, ID3D11Resource* pTexture, D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc,
			UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		Texture(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		Texture(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		Texture(ID3D11Device* device, const SE_Color& color, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);
		Texture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, UINT slot = 0u,
			Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER);

		~Texture() override {};

		void Bind(ID3D11DeviceContext* context) noexcept override;
		void Unbind(ID3D11DeviceContext* context) noexcept override;
		bool HasAlpha() const noexcept;
	private:
		void Initialize1x1ColorTexture(ID3D11Device* device, const SE_Color& colorData);
		void InitializeColorTexture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height);

		UINT slot;
		std::string filePath;
		bool hasAlpha = false;
		Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;

		Bind::PipelineStage pipelineStage = Bind::PipelineStage::PIXEL_SHADER;
	};

}
