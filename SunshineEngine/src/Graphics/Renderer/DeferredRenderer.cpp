#include <Graphics/Renderer/DeferredRenderer.h>
#include <iostream>

namespace SE_G {
	DeferredRenderer::DeferredRenderer(
		eastl::string name,
		ID3D11Device* device, ID3D11DeviceContext* context,
		UINT screenWidth, UINT screenHeight)
		: RenderGroup(name, device, context)
	{	
		this->m_screenWidth = screenWidth;
		this->m_screenHeight = screenHeight;

		this->m_context = context;
		this->m_device = device;

		InitGBuffer(m_screenWidth, m_screenHeight);
	}

	DeferredRenderer::~DeferredRenderer() {

	}

	void DeferredRenderer::InitGBuffer(UINT screenWidth, UINT screenHeight)
	{
		m_GBuffer = eastl::make_shared<GBuffer>(m_device, screenWidth, screenHeight);
		m_mainCamera = eastl::make_shared<Camera>(m_device, screenWidth * 1.0f / screenHeight);
		m_mainCamera->SetPosition({ 0, 0, -10 });
	}

	void DeferredRenderer::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;
		m_GBuffer->OnResize(GetDevice(), resizeWidth, resizeHeight);

		// Passes
		for (auto& pass : m_passes) {
			pass->OnResize(m_screenWidth, m_screenHeight);

		}
	}

	void DeferredRenderer::Pass()
	{
		// Passes
		for (UINT i = 0u; i < static_cast<UINT>(RenderPass::PassType::Count); i++)
		{
			if (m_passesOrder.contains(static_cast<RenderPass::PassType>(i)))
			{
				auto idx = m_passesOrder[static_cast<RenderPass::PassType>(i)];

				auto& pass = m_passes[idx];
				if (!pass->IsEnabled())
					continue;

				m_context->ClearState();

				pass->StartFrame();
				pass->Pass();
				pass->EndFrame();
			}
		}

		/*
		for (auto& pass : m_passes) {
			if (!pass->IsEnabled())
				continue;

			m_context->ClearState();

			pass->StartFrame();
			pass->Pass();
			pass->EndFrame();
		}
		*/
	}

	RenderPass* DeferredRenderer::AddPass(eastl::unique_ptr<RenderPass> pass) {
		auto passType = pass->m_passType;
		m_passesOrder[passType] = m_passes.size();

		m_passes.push_back(eastl::move(pass));
		return m_passes.back().get();
	}

	RenderPass* DeferredRenderer::GetPass(RenderPass::PassType passType)
	{
		if (!m_passesOrder.contains(passType))
		{
			return nullptr;
		}
		else
		{
			return m_passes[m_passesOrder[passType]].get();
		}
	}

}