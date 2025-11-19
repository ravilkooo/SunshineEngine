#include <Graphics/Renderer/RenderGroup.h>

namespace SE_G {
	RenderGroup::RenderGroup(
		eastl::string name, ID3D11Device* device, ID3D11DeviceContext* context)
		: m_groupName(name), m_device(device), m_context(context)
	{}

	RenderGroup::~RenderGroup()
	{
		m_groupName.clear();
		m_passes.clear();
	}

	ID3D11Device* RenderGroup::GetDevice() {
		return m_device;
	}

	ID3D11DeviceContext* RenderGroup::GetDeviceContext() {
		return m_context;
	}

	// == void RenderGroup::RenderScene()
	void RenderGroup::Pass()
	{
		// Passes
		for (auto& pass : m_passes) {
			if (!pass->IsEnabled())
				continue;

			m_context->ClearState();

			pass->StartFrame();
			pass->Pass();
			pass->EndFrame();
		}
	}

	RenderPass* RenderGroup::AddPass(eastl::unique_ptr<RenderPass> pass) {
		m_passes.push_back(eastl::move(pass));
		return m_passes.back().get();
	}

	RenderTechnique* RenderGroup::AddTechnique(eastl::unique_ptr<RenderTechnique> tech) {
		for (auto& pass : m_passes)
		{
			if (pass->GetTechniqueTag() == tech->GetTechniqueTag())
			{
				return pass->AddTechnique(eastl::move(tech));
			}
		}
		// log << ("RenderGroup %s has not pass with %s tag", m_groupName, tech->GetTechniqueTag())
		return nullptr;
	}

	bool RenderGroup::IsEnabled() {
		return m_enabled;
	}

	void RenderGroup::Disable() {
		m_enabled = false;
	}

	void RenderGroup::Enable() {
		m_enabled = true;
	}
}
