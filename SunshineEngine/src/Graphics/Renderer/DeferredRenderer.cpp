#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/Renderer/Technique/GPassTechnique.h>

#include <ParticleSystem/ParticleSystem.h>

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
		//m_passes.clear();
	}

	void DeferredRenderer::InitGBuffer(UINT screenWidth, UINT screenHeight)
	{
		m_GBuffer = eastl::make_shared<GBuffer>(m_device, screenWidth, screenHeight);
		m_mainCamera = eastl::make_shared<Camera>(m_device, screenWidth * 1.0f / screenHeight);
		m_mainCamera->SetPosition({ 0, 0, -10 });
	}

	void DeferredRenderer::InitParticleSystem()
	{
		m_particleSystem = eastl::make_shared<SE::ParticleSystem>(
			this, GetMainCamera());
	}

	void DeferredRenderer::SetParticleSystem(eastl::shared_ptr<SE::ParticleSystem> ps)
	{
		m_particleSystem = ps;
		m_particleSystem->SetRenderer(this);
		m_particleSystem->SetCamera(this->GetMainCamera());
	}

	void DeferredRenderer::SetMainCamera(eastl::shared_ptr<Camera> camera)
	{ 
		m_mainCamera = camera;
		
		for (auto& pass : m_passes) {
			pass.second->SetCamera(m_mainCamera);
		}
	}

	eastl::shared_ptr<Camera> DeferredRenderer::GetMainCamera()
	{ 
		return m_mainCamera;
	};

	void DeferredRenderer::OnResize(UINT resizeWidth, UINT resizeHeight)
	{
		m_screenWidth = resizeWidth;
		m_screenHeight = resizeHeight;
		m_GBuffer->OnResize(GetDevice(), resizeWidth, resizeHeight);

		// Passes
		for (auto& pass : m_passes) {
			pass.second->OnResize(m_screenWidth, m_screenHeight);

		}
	}

	void DeferredRenderer::Pass()
	{
		// Passes
		for (UINT i = 0u; i < static_cast<UINT>(RenderPass::PassType::Count); i++)
		{
			if (m_passes.contains(static_cast<RenderPass::PassType>(i)))
			{
				auto pass = m_passes[static_cast<RenderPass::PassType>(i)].get();

				if (!pass->IsEnabled())
					continue;

				m_context->ClearState();

				pass->StartFrame();
				pass->Pass();
				pass->EndFrame();
			}
		}
	}
}
