#include <PlayerObject/MiniViewRenderer.h>
#include <Graphics/Renderer/Pass/GPass.h>
#include <Graphics/Renderer/Pass/ShadowMapPass.h>
#include <Graphics/Renderer/Pass/LightPass.h>

namespace SE_G {
	MiniViewRenderer::MiniViewRenderer(eastl::string name, ID3D11Device* device, ID3D11DeviceContext* context) :
		DeferredRenderer(name, device, context, 640u, 360u)
	{

	}

	MiniViewRenderer::~MiniViewRenderer()
	{
		m_parentRenderer = nullptr;
	}

	void MiniViewRenderer::SetParentRenderer(DeferredRenderer* parentRenderer)
	{
		m_parentRenderer = parentRenderer;

		// Passes
		for (UINT i = 0u; i < static_cast<UINT>(RenderPass::PassType::Count); i++)
		{
			if (!parentRenderer->m_passes.contains(static_cast<RenderPass::PassType>(i)))
				continue;
			auto passType = static_cast<RenderPass::PassType>(i);

			switch (passType)
			{
			case (SE_G::RenderPass::PassType::GPass): {
				auto pass = static_cast<SE_G::GPass*>(parentRenderer->m_passes[passType].get());
				this->AddPass(eastl::make_unique<SE_G::GPass>(
					GetDevice(), GetDeviceContext(),
					m_GBuffer, GetMainCamera()));

				break;
			}
			case (SE_G::RenderPass::PassType::Shadow): {
				break;
			}
			case (SE_G::RenderPass::PassType::Light): {
				auto pass = static_cast<SE_G::LightPass*>(parentRenderer->m_passes[passType].get());
				AddPass(eastl::make_unique<SE_G::LightPass>(
					GetDevice(), GetDeviceContext(),
					m_GBuffer, GetMainCamera()));

				break;
			}
			case (SE_G::RenderPass::PassType::Collider): {
				break;
			}
			case (SE_G::RenderPass::PassType::Icon): {
				break;
			}
			case (SE_G::RenderPass::PassType::Selection): {
				break;
			}
			}

		}
	}
	
	void MiniViewRenderer::Pass()
	{
		for (UINT i = 0u; i < static_cast<UINT>(RenderPass::PassType::Count); i++)
		{
			if (m_passes.contains(static_cast<RenderPass::PassType>(i)))
			{
				auto pass = m_passes[static_cast<RenderPass::PassType>(i)].get();

				if (!pass->IsEnabled())
					continue;

				m_context->ClearState();

				pass->StartFrame();
				
				auto parentPass = m_parentRenderer->m_passes[static_cast<RenderPass::PassType>(i)].get();
				parentPass->Pass();

				pass->EndFrame();
			}
		}
	}
}