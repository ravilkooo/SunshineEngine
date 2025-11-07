#include "Graphics/Renderer/Pass/RenderPass.h"

namespace SE_G {
	RenderPass::RenderPass(eastl::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context)
		: techniqueTag(techniqueTag), device(device), context(context)
	{
	}

	eastl::string RenderPass::GetTechnique()
	{
		return techniqueTag;
	}

	void RenderPass::Pass(const Scene& scene)
	{
		BindAllPerFrame();

		for (const auto& gameObjectUUID : scene.gameObjects) {
			const auto& gameObject = scene.GetGameObjectByUUID(gameObjectUUID);
			if (gameObject->HasComponent<RenderComponent>() &&
				gameObject->HasComponent<TransformComponent>()) {

				auto renderComponent = gameObject->GetComponent<RenderComponent>();

				if (!renderComponent->HasTechnique(techniqueTag))
					continue;

				gameObject->GetComponent<TransformComponent>()->BindToGraphicsPipeline(GetDeviceContext());
				renderComponent->PassTechnique(techniqueTag, GetDeviceContext());

			}
		}
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
}