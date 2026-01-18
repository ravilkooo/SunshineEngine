#include "Graphics/Renderer/Technique/GPassTechnique.h"
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/BlendState.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Component/MeshComponent.h>
#include <Utils/StringUtils.h>
#include <ResourceManager/ResourceManagerFacade.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>


namespace SE_G {
	GPassTechnique::GPassTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
		SE::UUID uuid)
		: RenderTechnique(device, assignedTransform, technique)
	{
		m_device = device;
		
		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		m_rasterizer = eastl::make_unique<Bind::Rasterizer>(device, rastDesc);

		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

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
		blendDesc.RenderTarget[4].BlendEnable = FALSE;
		blendDesc.RenderTarget[4].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_blendState = eastl::make_unique<Bind::BlendState>(device, blendDesc);

		m_uuidBuffer = eastl::make_unique<Bind::PixelConstantBuffer<SE::UUIDhilo>>(
			device,
			uuid.GetHilo(),
			0u
		);

		//eastl::string vertexShaderPath = "Shaders/GPass/GPassShaderVS.hlsl";
		//ResourceHandle vertexHandle = ResourceManagerFacade::Instance().LoadByPath(vertexShaderPath);
		//SE_G::Bind::VertexShader* shader = ResourceManagerFacade::Instance().Get<SE_G::Bind::VertexShader>(vertexHandle);
		//if (!shader)
		//{
		//	printSunshineErrorMessage("VertexShader not loaded (possibly wrong path or loader).");
		//	// Задай m_vertexShader = nullptr и не вызывай Bind!
		//}
		//else
		//{
		//	m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(shader, {});
		//}



		m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
			device, MakeEngineAssetPath_Wstring(L"Shaders/GPass/GPassShaderVS.hlsl").c_str());

		//m_colored = true;
		m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
			device, MakeEngineAssetPath_Wstring(L"Shaders/GPass/GPassTextureShaderPS.hlsl").c_str());

		
		//eastl::string texturePath = "Assets/Textures/UnloadedTextureColor.dds";
		//// Загружаем и получаем handle
		//ResourceHandle texHandle = ResourceManagerFacade::Instance().LoadByPath(texturePath);
		//// Получаем указатель на Texture
		//SE_G::Bind::Texture* tex = ResourceManagerFacade::Instance().Get<SE_G::Bind::Texture>(texHandle);
		//// Сохраняем для дальнейшего использования в рендере (в shared_ptr, если требуется)
		//m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(tex, [](SE_G::Bind::Texture*) {});

		m_texture = eastl::make_shared<SE_G::Bind::Texture>(device,
			SE_G::Colors::UnloadedTextureColor,
			0u,
			SE_G::Bind::PipelineStage::PIXEL_SHADER);

		m_textureSampler = eastl::make_unique<SE_G::Bind::Sampler>(
			device,
			SE_G::Bind::SamplerPreset::Wrap);
			device, MakeEngineAssetPath_Wstring(L"Shaders/GPass/GPassTextureShaderPS.hlsl").c_str();
	}

	GPassTechnique::~GPassTechnique()
	{
		
	}

GPassTechnique::GPassTechnique(GPassTechnique&& other) noexcept
	: RenderTechnique(eastl::move(other)),
	  m_uuidBuffer(eastl::move(other.m_uuidBuffer)),
	  m_device(other.m_device)
{
	other.m_device = nullptr;
}

GPassTechnique& GPassTechnique::operator=(GPassTechnique&& other) noexcept
{
	if (this != &other) {
		RenderTechnique::operator=(eastl::move(other));
		m_uuidBuffer = eastl::move(other.m_uuidBuffer);
		m_device = other.m_device;
		other.m_device = nullptr;
	}
	return *this;
}

	void GPassTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		RenderTechnique::BindAll(context);
		m_uuidBuffer->Bind(context.Get());
		m_meshData->m_mesh->Bind(context.Get());
		m_meshData->m_texture->Bind(context.Get(), 0u);
		m_meshData->m_textureSampler->Bind(context.Get());
	}

	void GPassTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		m_meshData->m_mesh->Draw(context.Get());
	}

	/*
	void GPassTechnique::SetColor(SE_G::Color color) {
		m_texture->ChangeColor(m_device, color);

		if (!m_colored)
		{
			m_colored = true;
		}
	}

	void GPassTechnique::SetTexture(const eastl::wstring& filePath,
		SE_G::Bind::SamplerPreset samplerPreset) {
		m_texture->ChangeTexture(m_device, filePath);

		if (m_colored)
		{
			m_colored = false;
		}

		m_textureSampler->ChangePreset(m_device, samplerPreset);
	}

	void GPassTechnique::ClearTexture() {
		m_texture->ClearTexture();
		SetColor(SE_G::Colors::UnloadedTextureColor);
	}
	*/

	void GPassTechnique::InitByMeshData(eastl::shared_ptr<MeshData> meshData)
	{
		m_meshData = meshData;
		// m_mesh = meshComponent->GetMesh();
		// m_texture = meshComponent->GetTexture();
		// m_textureSampler = meshComponent->GetTextureSamplerPreset();
	}

	/*
	void GPassTechnique::SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh) {
		m_mesh->ClearMesh();
		m_mesh = newMesh;
	}

	void GPassTechnique::SetMesh(const eastl::string& filePath)
	{
		eastl::shared_ptr<SE_G::Mesh> newMesh = eastl::make_shared<SE_G::Mesh>(
			m_device, filePath);
		m_mesh->ClearMesh();
		m_mesh = newMesh;
		//m_mesh->ChangeMesh(device, filePath);
	}
	*/

	/*
	void GPassTechnique::ClearMesh() {
		
	}
	*/
}