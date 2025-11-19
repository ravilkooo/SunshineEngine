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
}