#include <Graphics/Bindable/Bindable.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/DepthStencilState.h>

#include <Graphics/GraphicsResources/GeometryShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/Pass/IconPass.h>
#include <Graphics/Renderer/Technique/IconTechnique.h>

#include <Utils/StringUtils.h>

namespace SE_G {
	IconPass::IconPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("IconPass", device, context)
	{
		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Icon;

		// Set RTVs
		m_bufferRTVs[0] = pGBuffer->pLightRTV.Get();
		m_bufferRTVs[1] = pGBuffer->pUUIDRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;

		D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

		AssetPath shaderPath = AssetPath(L"Shaders/IconPass/IconShaderVS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::VERTEX_SHADER;
		shaderPath.m_params.asShader.numInputElements = 2;
		shaderPath.m_params.asShader.IALayoutInputElements = new D3D11_INPUT_ELEMENT_DESC[shaderPath.m_params.asShader.numInputElements];
		shaderPath.m_params.asShader.IALayoutInputElements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		shaderPath.m_params.asShader.IALayoutInputElements[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		auto& rm = ResourceManagerFacade::Instance();
		ResourceHandle iconVshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::VertexShader* iconVshaderRes = rm.Get<SE_G::Bind::VertexShader>(iconVshaderHandle);
		m_iconVertexShader = eastl::shared_ptr<SE_G::Bind::VertexShader>(
			iconVshaderRes,
			[](SE_G::Bind::VertexShader*) {}
		);
		delete[] shaderPath.m_params.asShader.IALayoutInputElements;
		/*
		UINT numInputElements = 2;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		m_iconVertexShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/IconPass/IconShaderVS.hlsl").c_str(),
			numInputElements, IALayoutInputElements);
		*/

		m_camGCB = eastl::make_unique<Bind::GeometryConstantBuffer<CamGCB>>
			(
				device,
				0u
			);

		AddPerFrameBind(m_camGCB.get());

		// m_iconGeometryShader = eastl::make_unique<Bind::GeometryShader>(device,
		// 	MakeEngineAssetPath_Wstring(L"Shaders/IconPass/IconShaderGS.hlsl").c_str());

		shaderPath = AssetPath(L"Shaders/IconPass/IconShaderGS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::GEOMETRY_SHADER;
		ResourceHandle gshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::GeometryShader* gshaderRes = rm.Get<SE_G::Bind::GeometryShader>(gshaderHandle);
		m_iconGeometryShader = eastl::shared_ptr<SE_G::Bind::GeometryShader>(
			gshaderRes,
			[](SE_G::Bind::GeometryShader*) {}
		);

		m_iconSprites = eastl::make_unique<Bind::Texture>(
			device,
			AssetPath(L"Textures/EditorIcons.dds", AssetPath::AssetSource::Engine), 0u,
			Bind::PipelineStage::PIXEL_SHADER
		);

		// Usual sampler for all SRV
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_GBufferSampler = eastl::make_unique<Bind::Sampler>(device, samplerDesc, 0u);
		AddPerFrameBind(m_GBufferSampler.get());

		m_spritesheetInfoPCB = eastl::make_unique<Bind::PixelConstantBuffer<SpritesheetInfoPCB>>(
			device,
			m_spritesheetData,
			0u);
		AddPerFrameBind(m_spritesheetInfoPCB.get());

		// m_iconPixelShader = eastl::make_unique<Bind::PixelShader>(device,
		// 	MakeEngineAssetPath_Wstring(L"Shaders/IconPass/IconShaderPS.hlsl").c_str());
		shaderPath = AssetPath(L"Shaders/IconPass/IconShaderPS.hlsl", AssetPath::AssetSource::Engine);
		shaderPath.m_params.asShader.shaderType = SE_G::Bind::PipelineStage::PIXEL_SHADER;
		ResourceHandle pshaderHandle = rm.LoadByPath(shaderPath);
		SE_G::Bind::PixelShader* pshaderRes = rm.Get<SE_G::Bind::PixelShader>(pshaderHandle);
		m_iconPixelShader = eastl::shared_ptr<SE_G::Bind::PixelShader>(
			pshaderRes,
			[](SE_G::Bind::PixelShader*) {}
		);
	}

	IconPass::~IconPass() {
		//delete[] m_bufferRTVs;
	}

	void IconPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Icon Pass");

		context->OMSetRenderTargets(2, m_bufferRTVs, m_GBuffer->pDepthDSV.Get());

		context->RSSetViewports(1, &m_viewport);
	}

	void IconPass::Pass()
	{
		BindAllPerFrame();
		m_iconVertexShader->Bind(context.Get());
		m_iconGeometryShader->Bind(context.Get());
		m_iconPixelShader->Bind(context.Get());
		m_iconSprites->Bind(context.Get());
		m_depthStencilState->Bind(context.Get());

		m_spritesheetInfoPCB->Bind(context.Get());

		m_camGCB->Update(GetDeviceContext(), {
			m_camera->GetViewMatrix(),
			m_camera->GetProjectionMatrix(),
			m_camera->GetPosition(), 0.0f });
		m_camGCB->Bind(context.Get());
		/*
		for (const auto& gameObjectUUID : scene.gameObjects) {
			const auto& gameObject = scene.GetGameObjectByUUID(gameObjectUUID);
			if (gameObject->HasComponent<RenderComponent>() &&
				gameObject->HasComponent<TransformComponent>()) {

				auto renderComponent = gameObject->GetComponent<RenderComponent>();

				if (!renderComponent->HasTechnique(techniqueTag))
					continue;

				gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());
				renderComponent->PassTechnique(techniqueTag, GetDeviceContext()); // Bind + Draw
			}
		}
		*/
		for (auto& tech : m_techniques) {


			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());
		}
	}

	void IconPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		context->OMSetRenderTargets(2, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void IconPass::OnResize(UINT resizeWidth, UINT resizeHeight)
		//eastl::shared_ptr<GBuffer> pGBuffer)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		//m_GBuffer = pGBuffer;

		// Set RTVs
		m_bufferRTVs[0] = m_GBuffer->pLightRTV.Get();
		m_bufferRTVs[1] = m_GBuffer->pUUIDRTV.Get();

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;
	}
}