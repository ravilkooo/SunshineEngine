#include "Graphics/Renderer/Technique/GPassTechnique.h"
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/BlendState.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>
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

		AssetPath shaderPath = AssetPath(L"Shaders/GPass/GPassShaderVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		SE_G::Bind::VertexShader::FillStandartInputLayout(shaderPath.m_params.asShader.numInputElements,
			shaderPath.m_params.asShader.IALayoutInputElements);
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle vshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* vshaderRes = rm.Get<SE_G::Bind::VertexShader>(vshaderHandle);
		m_vertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			vshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;
		// m_vertexShader = eastl::make_shared<SE_G::Bind::VertexShader>(
		// 	device, MakeEngineAssetPath_Wstring(L"Shaders/GPass/GPassShaderVS.hlsl").c_str());

		//m_colored = true;
		// m_pixelShader = eastl::make_shared<SE_G::Bind::PixelShader>(
		// 	device, MakeEngineAssetPath_Wstring(L"Shaders/GPass/GPassTextureShaderPS.hlsl").c_str());
		shaderPath = AssetPath(L"Shaders/GPass/GPassTextureShaderPS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
		ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
		m_pixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
			pshaderRes,
			[](SE_G::Bind::PixelShader*) {}
		);

		auto ap = AssetPath(
			SE_G::Bind::Texture::ColorToPath(SE_G::Colors::UnloadedTextureColor),
			AssetPath::AssetSource::Engine);
		ResourceHandle texHandle = ResourceManagerFacade::Instance().LoadByPath(ap);
		SE_G::Bind::Texture* texture =
			ResourceManagerFacade::Instance().Get<SE_G::Bind::Texture>(texHandle);

		m_texture = eastl::shared_ptr<SE_G::Bind::Texture>(
			texture,
			[](SE_G::Bind::Texture*) {}
		);
		m_texture->m_texturePath = texture->m_texturePath;

		m_textureSampler = eastl::make_unique<SE_G::Bind::Sampler>(
			device,
			SE_G::Bind::SamplerPreset::Wrap);
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

	void GPassTechnique::SetRasterizer(D3D11_RASTERIZER_DESC rastDesc)
	{
		m_rasterizer = eastl::make_unique<Bind::Rasterizer>(m_device, rastDesc);
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

	void GPassTechnique::InitByMeshData(eastl::shared_ptr<MeshData> meshData)
	{
		m_meshData = meshData;
		// m_mesh = meshComponent->GetMesh();
		// m_texture = meshComponent->GetTexture();
		// m_textureSampler = meshComponent->GetTextureSamplerPreset();
	}

}