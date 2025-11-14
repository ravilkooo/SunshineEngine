#include "Graphics/Renderer/Technique/GPassTechnique.h"
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Utils/StringUtils.h>

namespace SE_G {
	GPassTechnique::GPassTechnique(DeferredRenderer* renderSystem, TransformComponent* assignedTransform, eastl::string technique,
		SE::UUID uuid)
		: RenderTechnique(renderSystem->GetDevice(), assignedTransform, technique)
	{
		m_renderSystem = renderSystem;
		
		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rasterizer = eastl::make_shared<Bind::Rasterizer>(renderSystem->GetDevice(), rastDesc);

		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilState = eastl::make_shared<Bind::DepthStencilState>(renderSystem->GetDevice(), dsDesc);

		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.IndependentBlendEnable = TRUE;
		for (size_t i = 0; i < 4; i++)
		{
			blendDesc.RenderTarget[i].BlendEnable = TRUE;
			blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		blendDesc.RenderTarget[4].BlendEnable = TRUE;

		blendState = eastl::make_shared<Bind::BlendState>(renderSystem->GetDevice(), blendDesc);

		m_uuidBuffer = eastl::make_shared<Bind::PixelConstantBuffer<UUIDhilo>>(
			renderSystem->GetDevice(),
			uuid.GetHilo(),
			0u
		);

		AddBind(m_uuidBuffer);

		m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
			renderSystem->GetDevice(), MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderVS.hlsl"));

		m_colored = true;
		m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
			renderSystem->GetDevice(), MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));
		m_texture = eastl::make_shared<SE_G::Bind::Texture>(renderSystem->GetDevice(),
			SE_G::Colors::UnloadedTextureColor,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);
		m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
			renderSystem->GetDevice(),
			SE_G::Bind::SamplerPreset::Wrap);
	}

	void GPassTechnique::SetColor(SE_G::Color color) {
		m_texture->ChangeColor(m_renderSystem->GetDevice(), color);

		if (!m_colored)
		{
			/*
			m_pixelShader->ChangeShader(m_renderSystem->GetDevice(),
				MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassShaderPS.hlsl"));
				*/
			m_colored = true;
		}
	}

	void GPassTechnique::SetTexture(const eastl::wstring& filePath,
		SE_G::Bind::SamplerPreset samplerPreset) {
		m_texture->ChangeTexture(m_renderSystem->GetDevice(), filePath);

		if (m_colored)
		{
			/*
			m_pixelShader->ChangeShader(m_renderSystem->GetDevice(),
				MakeEngineAssetPath_Wchar(L"Shaders/GPass/GPassTextureShaderPS.hlsl"));
				*/
			m_colored = false;
		}

		m_textureSampler->ChangeSampler(m_renderSystem->GetDevice(), samplerPreset);
	}

	void GPassTechnique::ClearTexture() {
		m_texture->ClearTexture();
		SetColor(SE_G::Colors::UnloadedTextureColor);
	}

	void GPassTechnique::SetMesh(const eastl::string& filePath) {
		m_mesh->ChangeMesh(m_renderSystem->GetDevice(), filePath);
	}

	void GPassTechnique::ClearMesh() {
		
	}
}