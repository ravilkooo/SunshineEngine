#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Bindable/Bindable.h>
#include <Component/TransformComponent.h>

namespace SE_G {
	using namespace Bind;

	RenderTechnique::RenderTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique)
		: m_techniqueTag(technique), m_assignedTransform(assignedTransform)
	{
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

		if (m_vertexShader)
			m_vertexShader->Bind(context.Get());

		if (m_pixelShader)
			m_pixelShader->Bind(context.Get());

		if (m_texture) {
			m_texture->Bind(context.Get());
		}

		if (m_textureSampler) {
			m_textureSampler->Bind(context.Get());
		}

		// Bind rasterizer
		rasterizer->Bind(context.Get());

		// Bind depthState
		depthStencilState->Bind(context.Get());

		if (blendState)
			blendState->Bind(context.Get());

		if (m_mesh)
			m_mesh->Bind(context.Get());
	}

	void RenderTechnique::DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		if (m_mesh)
			m_mesh->Draw(context.Get());

	}

	eastl::string RenderTechnique::GetTechniqueTag()
	{
		return m_techniqueTag;
	}
}