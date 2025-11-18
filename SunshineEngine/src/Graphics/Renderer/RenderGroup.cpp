#include <Graphics/Renderer/RenderGroup.h>

namespace SE_G {
	RenderGroup::RenderGroup(
		eastl::string name, ID3D11Device* device, ID3D11DeviceContext* context)
		: m_groupName(name), m_device(device), m_context(context)
	{}

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
		for (auto pass : m_passes) {
			if (!pass->IsEnabled())
				continue;

			m_context->ClearState();

			pass->StartFrame();
			pass->Pass();
			pass->EndFrame();
		}
	}

	void RenderGroup::AddPass(eastl::shared_ptr<RenderPass> pass) {
		m_passes.push_back(pass);
	}

	void RenderGroup::AddTechnique(eastl::unique_ptr<RenderTechnique> tech) {
		for (auto pass : m_passes)
		{
			if (pass->GetTechniqueTag() == tech->GetTechniqueTag())
			{
				pass->AddTechnique(eastl::move(tech));
				return;
			}
		}
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
