#include "Graphics/Renderer/Technique/RenderTechnique.h"
#include "Graphics/Bindable/Bindable.h"

namespace SE_G {
	using namespace Bind;

	RenderTechnique::RenderTechnique(ID3D11Device* device, eastl::string technique)
		: techniqueTag(technique)
	{
	}

	RenderTechnique::~RenderTechnique()
	{
		m_techniqueTag.clear();
		bindables.clear();
	}

	void RenderTechnique::AddBind(eastl::shared_ptr<Bind::Bindable> bind)
	{
		bindables.push_back(bind);
	}

	void RenderTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void RenderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		for (size_t i = 0; i < bindables.size(); i++)
		{
			bindables[i]->Bind(context.Get());
		}

		if (vertexShader)
			vertexShader->Bind(context.Get());

		if (pixelShader)
			pixelShader->Bind(context.Get());

		if (texture) {
			texture->Bind(context.Get());
		}

		if (textureSampler) {
			textureSampler->Bind(context.Get());
		}

		// Bind rasterizer
		rasterizer->Bind(context.Get());

		// Bind depthState
		depthStencilState->Bind(context.Get());

		if (blendState)
			blendState->Bind(context.Get());

		if (mesh)
			mesh->Bind(context.Get());
	}

	void RenderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		if (mesh)
			mesh->Draw(context.Get());

	}

	eastl::string RenderTechnique::GetTechnique()
	{
		return techniqueTag;
	}
}