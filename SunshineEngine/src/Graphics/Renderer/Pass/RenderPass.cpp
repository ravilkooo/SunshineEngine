#include "Graphics/Renderer/Pass/RenderPass.h"

namespace SE_G {
	RenderPass::RenderPass(eastl::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context)
		: techniqueTag(techniqueTag), device(device), context(context)
	{
	}

	RenderPass::~RenderPass()
	{
		techniqueTag.clear();

		perFrameBindables.clear();

		m_techniques.clear();
	}

	eastl::string RenderPass::GetTechniqueTag()
	{
		return techniqueTag;
	}

	void RenderPass::Pass()
	{
		BindAllPerFrame();

		for (auto& tech : m_techniques) {
			tech->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech->Pass(GetDeviceContext());
		}
	}

	RenderTechnique* RenderPass::AddTechnique(eastl::unique_ptr<RenderTechnique> tech) {
		m_techniques.push_back(eastl::move(tech));
		return m_techniques.back().get();
	}

	void RenderPass::AddPerFrameBind(Bind::Bindable* bind)
	{
		perFrameBindables.push_back(bind);
	}

	void RenderPass::BindAllPerFrame()
	{
		for (size_t i = 0; i < perFrameBindables.size(); i++)
		{
			perFrameBindables[i]->Bind(context.Get());
		}
	}

	ID3D11Device* RenderPass::GetDevice()
	{
		return device.Get();
	}

	ID3D11DeviceContext* RenderPass::GetDeviceContext()
	{
		return context.Get();
	}

	bool RenderPass::IsEnabled() {
		return m_enabled;
	}

	void RenderPass::Disable() {
		m_enabled = false;
	}

	void RenderPass::Enable() {
		m_enabled = true;
	}

}