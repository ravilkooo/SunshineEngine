#include "Graphics/Renderer/Pass/RenderPass.h"

#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

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

	void RenderPass::ClearTechniques()
	{
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
			tech.second->m_assignedTransform->BindToGraphicsPipeline(GetDeviceContext());
			tech.second->Pass(GetDeviceContext());
		}
	}

	RenderTechnique* RenderPass::AddTechnique(SE::UUID uuid, eastl::unique_ptr<RenderTechnique> tech)
	{
		const SE::UUID id = uuid;
		auto [it, inserted] = m_techniques.emplace(id, nullptr);
		if (!inserted)
		{
			printf("Duplicate UUID in RenderPass::AddGameObject\n");
			//return nullptr;
		}
		it->second = std::move(tech);
		return m_techniques[id].get();
	}

	RenderTechnique* RenderPass::GetTechnique(SE::UUID uuid)
	{
		auto it = m_techniques.find(uuid);
		if (it == m_techniques.end())
			return nullptr;

		return it->second.get();
	}

	void RenderPass::RemoveTechnique(SE::UUID uuid)
	{
		auto it = m_techniques.find(uuid);
		if (it == m_techniques.end())
			return;

		m_techniques.erase(it);
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

	eastl::shared_ptr<Camera> RenderPass::GetCamera()
	{
		return m_camera;
	}

	void RenderPass::SetCamera(eastl::shared_ptr<Camera> camera)
	{
		this->m_camera = camera;
	}

}