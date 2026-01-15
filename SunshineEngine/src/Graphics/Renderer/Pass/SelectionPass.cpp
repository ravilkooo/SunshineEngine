#include <Graphics/Renderer/RenderingSystem.h>

#include <Graphics/Renderer/Technique/IconTechnique.h>
#include <Graphics/Renderer/Pass/SelectionPass.h>
#include <Graphics/Renderer/Pass/IconPass.h>
#include <Graphics/GraphicsResources/GeometryShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Bindable/Sampler.h>

#include <Utils/StringUtils.h>

namespace SE_G {
	SelectionPass::SelectionPass(ID3D11Device* device, ID3D11DeviceContext* context,
		eastl::shared_ptr<GBuffer> pGBuffer,
		eastl::shared_ptr<Camera> camera)
		:
		RenderPass("SelectionPass", device, context)
	{
		this->m_GBuffer = pGBuffer;
		this->m_camera = camera;
		this->m_screenWidth = pGBuffer->m_screenWidth;
		this->m_screenHeight = pGBuffer->m_screenHeight;
		m_passType = PassType::Selection;

		// Viewport
		m_viewport = {};
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.MinDepth = 0;
		m_viewport.MaxDepth = 1.0f;

		/*
		m_screenInfoPCB = new Bind::PixelConstantBuffer<ScreenInfoPCB>(device,
			{ DXSM::Vector2(
				static_cast<float>(m_screenWidth),
				static_cast<float>(m_screenHeight)) },
			1u);
		AddPerFrameBind(m_screenInfoPCB);
		*/

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

		D3D11_DEPTH_STENCILOP_DESC stencil_op = {};
		stencil_op.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
		stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
		stencil_op.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		stencil_op.StencilFunc = D3D11_COMPARISON_ALWAYS;
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = false;
		desc.StencilEnable = true;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace = stencil_op;
		desc.BackFace = stencil_op;
		device->CreateDepthStencilState(&desc, m_depthStencilWriteMask.GetAddressOf());

		stencil_op = {};
		stencil_op.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		stencil_op.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencil_op.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
		desc = {};
		desc.DepthEnable = false;
		desc.StencilEnable = true;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.FrontFace = stencil_op;
		desc.BackFace = stencil_op;
		device->CreateDepthStencilState(&desc, m_depthStencilReadMask.GetAddressOf());

		m_meshVertexShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/SelectionPass/SelectionMeshShaderVS.hlsl").c_str());

		UINT numInputElements = 2;
		D3D11_INPUT_ELEMENT_DESC IALayoutInputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		m_iconVertexShader = eastl::make_unique<Bind::VertexShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/SelectionPass/SelectionIconShaderVGS.hlsl").c_str(),
			numInputElements, IALayoutInputElements);

		m_selectionBuffer = eastl::make_unique<Bind::GeometryConstantBuffer<float>>(device, 1u);

		m_iconGeometryShader = eastl::make_unique<Bind::GeometryShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/SelectionPass/SelectionIconShaderVGS.hlsl").c_str());

		m_pixelShader = eastl::make_unique<Bind::PixelShader>(device,
			MakeEngineAssetPath_Wstring(L"Shaders/SelectionPass/SelectionMeshShaderPS.hlsl").c_str());

		m_selectedObjectUUID = SE::UUID(0u);
	}

	SelectionPass::~SelectionPass()
	{

	}

	void SelectionPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"Selection Pass");
	}

	void SelectionPass::Pass()
	{
		if (m_selectedObjectUUID == SE::UUID(0u))
			return;

		/*
		m_screenInfoPCB->Update(GetDeviceContext(), { DXSM::Vector2(
			static_cast<float>(m_screenWidth),
			static_cast<float>(m_screenHeight)) });
		*/

		BindAllPerFrame();

		GameObject_Info* gameObject_info = m_scene->GetGameObjectByUUID(m_selectedObjectUUID);

		if (gameObject_info->HasComponent<RenderComponent_Info>() &&
			gameObject_info->HasComponent<TransformComponent_Info>()) {

			auto renderComp_info = gameObject_info->GetComponent<RenderComponent_Info>();
			auto transformComponent = gameObject_info->GetComponent<TransformComponent_Info>()->m_assignedComponent.get();

			if (!renderComp_info->HasTechnique("GPass") && !renderComp_info->HasTechnique("IconPass")
				|| !(renderComp_info->m_selectionTechnique))
			{
				return;
			}

			auto actualLocalScaleFactor = DXSM::Vector3(transformComponent->m_localScaleFactor);

			renderComp_info->m_selectionTechnique->BindAll(context.Get());


			context->OMSetDepthStencilState(m_depthStencilWriteMask.Get(), 1); // draw scaled

			//context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
			ID3D11RenderTargetView* renderTargets[1] = { 0 };
			context->OMSetRenderTargets(1, renderTargets, m_GBuffer->pDepthDSV.Get());

			context->RSSetViewports(1, &m_viewport);

			m_camera->BindBuffer(context.Get());

			transformComponent->BindToGraphicsPipeline(
				GetDeviceContext()
			);

			if (renderComp_info->HasTechnique("GPass")) {
				//renderComp_info->techniques["GPass"]->mesh->Bind(context.Get());

				m_meshVertexShader->Bind(context.Get());
				context->PSSetShader(nullptr, nullptr, 0u);
				//context->RSSetState(nullptr, 0u);

				//renderComp_info->techniques["GPass"]->mesh->Draw(context.Get());
				renderComp_info->m_selectionTechnique->DrawTechnique(context.Get());

				// Step2 (draw color and mask out)
				// MaskOut
				renderComp_info->m_selectionTechnique->BindAll(context.Get());
				context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
				context->RSSetViewports(1, &m_viewport);

				context->OMSetDepthStencilState(m_depthStencilReadMask.Get(), 1);
				m_pixelShader->Bind(context.Get());

				transformComponent->m_localScaleFactor *= 1.08f;
				transformComponent->BindToGraphicsPipeline(
					GetDeviceContext()
				);

				//renderComp_info->techniques["GPass"]->mesh->Draw(context.Get());
				renderComp_info->m_selectionTechnique->DrawTechnique(context.Get());

				transformComponent->m_localScaleFactor = actualLocalScaleFactor;
			}
			else if (renderComp_info->HasTechnique("IconPass")) {

				// Bind IconSelectionShader (VertexShader)
				// Don't bind object texture and sampler for this
				m_iconVertexShader->Bind(context.Get());
				m_iconGeometryShader->Bind(context.Get());
				m_iconPass->m_camGCB->Bind(context.Get());

				m_selectionBuffer->Update(context.Get(), { 1.0f });
				m_selectionBuffer->Bind(context.Get());

				context->PSSetShader(nullptr, nullptr, 0u);
				renderComp_info->m_selectionTechnique->DrawTechnique(context.Get());

				// Step2 (draw color and mask out)
				// MaskOut
				context->OMSetRenderTargets(1, m_GBuffer->pLightRTV.GetAddressOf(), m_GBuffer->pDepthDSV.Get());
				context->RSSetViewports(1, &m_viewport);

				context->OMSetDepthStencilState(m_depthStencilReadMask.Get(), 1);
				m_pixelShader->Bind(context.Get());

				// scale width and height
				m_selectionBuffer->Update(context.Get(), { 1.08f });
				m_selectionBuffer->Bind(context.Get());

				renderComp_info->m_selectionTechnique->BindAll(context.Get());
				renderComp_info->m_selectionTechnique->DrawTechnique(context.Get());

				transformComponent->m_localScaleFactor = actualLocalScaleFactor;
			}
		}

		context->OMSetDepthStencilState(nullptr, 0);

	}

	void SelectionPass::EndFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void SelectionPass::OnResize(UINT resizeWidth, UINT resizeHeight)
		//eastl::shared_ptr<GBuffer> pGBuffer)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;

		//m_GBuffer = pGBuffer;

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
