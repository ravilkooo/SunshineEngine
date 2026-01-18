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
			if (!pass.second->IsEnabled())
				continue;

			m_context->ClearState();

			pass.second->StartFrame();
			pass.second->Pass();
			pass.second->EndFrame();
		}
	}

	RenderPass* RenderGroup::AddPass(eastl::unique_ptr<RenderPass> pass)
	{
		const SE_G::RenderPass::PassType passType = pass->m_passType;
		auto [it, inserted] = m_passes.emplace(passType, nullptr);
		if (!inserted)
		{
			printf("Duplicate Pass in RenderGroup::AddPass\n");
			//return nullptr;
		}
		it->second = std::move(pass);
		return m_passes[passType].get();
	}

	RenderPass* RenderGroup::GetPass(RenderPass::PassType passType)
	{
		if (!m_passes.contains(passType))
		{
			return nullptr;
		}
		else
		{
			return m_passes[passType].get();
		}
	}

	void RenderGroup::RemovePass(RenderPass::PassType passType)
	{
		auto it = m_passes.find(passType);
		if (it == m_passes.end())
			return;

		m_passes.erase(it);
	}

	RenderTechnique* RenderGroup::AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech)
	{
		for (auto& pass : m_passes)
		{
			if (pass.second->GetTechniqueTag() == tech->GetTechniqueTag())
			{
				return pass.second->AddTechnique(uuid, eastl::move(tech));
			}
		}
		// log << ("RenderGroup %s has not pass with %s tag", m_groupName, tech->GetTechniqueTag())
		return nullptr;
	}
	
	RenderTechnique* RenderGroup::GetTechnique(SE::UUID uuid, eastl::string techniqueTag)
	{
		for (auto& pass : m_passes)
		{
			if (pass.second->GetTechniqueTag() == techniqueTag)
			{
				return pass.second->GetTechnique(uuid);
			}
		}
		return nullptr;
	}

	void RenderGroup::RemoveTechnique(SE::UUID uuid, eastl::string techniqueTag)
	{
		for (auto& pass : m_passes)
		{
			if (pass.second->GetTechniqueTag() == techniqueTag)
			{
				return pass.second->RemoveTechnique(uuid);
			}
		}
	}

	void RenderGroup::RemoveAllTechniques(SE::UUID uuid)
	{
		for (auto& pass : m_passes)
		{
			pass.second->RemoveTechnique(uuid);
		}
	}
	
	void RenderGroup::ClearAllTechniques()
	{
		for (auto& pass : m_passes)
		{
			pass.second->ClearTechniques();
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
