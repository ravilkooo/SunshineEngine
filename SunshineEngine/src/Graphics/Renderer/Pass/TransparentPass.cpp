#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/RenderingSystem.h>
#include <Graphics/Renderer/GBuffer.h>
#include "Graphics/Renderer/Pass/TransparentPass.h"
#include <Graphics/Renderer/Technique/RenderTechnique.h>

#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/BlendState.h>

#include <Graphics/Utils/Camera.h>

#include <Component/TransformComponent.h>

namespace SE_G {
	TransparentPass::TransparentPass(DeferredRenderer* renderer,
		eastl::shared_ptr<GBuffer> pGBuffer)
		:
		RenderPass("Transparent", renderer)
	{
		auto device = renderer->GetDevice();

		this->screenWidth = pGBuffer->m_screenWidth;
		this->screenHeight = pGBuffer->m_screenHeight;
		this->pGBuffer = pGBuffer;
		m_passType = PassType::Transparent;

		m_objectsOrder = eastl::vector<TransparentPassData>();

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_depthStencilState = eastl::make_unique<Bind::DepthStencilState>(device, dsDesc);

		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.IndependentBlendEnable = TRUE;

		for (size_t i = 0; i < 3; i++)
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
		blendDesc.RenderTarget[1].BlendEnable = TRUE;
		blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc.RenderTarget[3].BlendEnable = FALSE;
		blendDesc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_blendState = eastl::make_unique<Bind::BlendState>(device, blendDesc);
		
		// Set RTVs
		gBufferRTVs[0] = pGBuffer->pNormalRTV.Get();
		gBufferRTVs[1] = pGBuffer->pLightRTV.Get();
		gBufferRTVs[2] = pGBuffer->pWorldPosRTV.Get();
		gBufferRTVs[3] = pGBuffer->pUUIDRTV.Get();

		// Viewport
		viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

	}

	TransparentPass::~TransparentPass()
	{
		//delete[] gBufferRTVs;
	}

	void TransparentPass::StartFrame()
	{
		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->BeginEvent(L"TransparentPass");
		auto context = m_renderer->GetDeviceContext();

		context->OMSetRenderTargets(4, gBufferRTVs, pGBuffer->pDepthDSV.Get());
		context->RSSetViewports(1, &viewport);
		m_depthStencilState->Bind(context);
		m_blendState->Bind(context);

		// Bind camera buffer to 1u slot
		m_renderer->GetMainCamera()->UpdateBuffer(context);
		m_renderer->GetMainCamera()->BindBuffer(context);

		// temporary solution to sort objects every frame, until we have a proper system to detect when an object is moved
		m_isDirty = true;
		if (m_isDirty)
		{
			SortObjects();
			m_isDirty = false;
		}
	}

	void TransparentPass::Pass()
	{
		BindAllPerFrame();

		for (auto& objData : m_objectsOrder) {
			auto it = m_techniques.find(objData.objectUUID);
			if (it != m_techniques.end()) {
				auto& tech = it->second;
				if (!tech->IsEnabled())
					continue;
				tech->m_assignedTransform->EnableMeshTransformMode();
				tech->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
				tech->Pass(GetDeviceContext());
				tech->m_assignedTransform->DisableMeshTransformMode();
			}
		}
	}

	void TransparentPass::EndFrame()
	{
		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		ID3D11DepthStencilView* nullDSVs[] = { nullptr };
		m_renderer->GetDeviceContext()->OMSetRenderTargets(4, nullRTVs, *nullDSVs);

		if (SE_G::RenderingSystem::gAnn) SE_G::RenderingSystem::gAnn->EndEvent();
	}

	void TransparentPass::SortObjects()
	{
		m_objectsOrder.clear();

		for (auto& tech : m_techniques) {
			if (!tech.second->IsEnabled())
				continue;
			TransparentPassData data;
			data.objectUUID = tech.first;
			data.pos = tech.second->m_assignedTransform->GetAbsoluteWorldPosition();
			m_objectsOrder.push_back(data);
		}

		std::sort(m_objectsOrder.begin(), m_objectsOrder.end(),
			[](const TransparentPassData& a, const TransparentPassData& b) {
				return a.pos.z > b.pos.z; // Sort by z position in descending order
			});
	}

	// GBuffer should be resized before this method
	void TransparentPass::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		screenWidth = resizeWidth;
		screenHeight = resizeHeight;

		// Set RTVs
		gBufferRTVs[0] = pGBuffer->pNormalRTV.Get();
		gBufferRTVs[1] = pGBuffer->pLightRTV.Get();
		gBufferRTVs[2] = pGBuffer->pWorldPosRTV.Get();
		gBufferRTVs[3] = pGBuffer->pUUIDRTV.Get();

		// Viewport
		viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;
	}
}