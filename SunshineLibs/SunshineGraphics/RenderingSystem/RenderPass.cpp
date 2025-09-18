

#include "RenderPass.h"

RenderPass::RenderPass(std::string techniqueTag, ID3D11Device* device, ID3D11DeviceContext* context)
	: techniqueTag(techniqueTag), device(device), context(context)
{
}

std::string RenderPass::GetTechnique()
{
	return techniqueTag;
}

void RenderPass::Pass(const Scene& scene)
{
	BindAllPerFrame();

	for (SceneNode* node : scene.nodes) {
		if (!node->HasTechnique(techniqueTag))
			continue;
		node->PassTechnique(techniqueTag, GetDeviceContext());
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



