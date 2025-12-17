#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Bindable/Bindable.h>
#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Bindable/BlendState.h>
#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/DepthStencilState.h>
#include <Component/TransformComponent.h>

namespace SE_G {
	using namespace Bind;

	RenderTechnique::RenderTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique)
		: m_assignedTransform(assignedTransform)
	{
		m_techniqueTag = technique;
	}

	RenderTechnique::~RenderTechnique()
	{
		m_techniqueTag.clear();
		m_bindables.clear();
	}

	void RenderTechnique::AddBind(eastl::shared_ptr<Bind::Bindable> bind)
	{
		m_bindables.push_back(bind);
	}

	void RenderTechnique::Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		BindAll(context);
		DrawTechnique(context);
	}

	void RenderTechnique::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		for (size_t i = 0; i < m_bindables.size(); i++)
		{
			m_bindables[i]->Bind(context.Get());
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
		if (m_rasterizer)
			m_rasterizer->Bind(context.Get());

		// Bind depthState
		if (m_depthStencilState)
			m_depthStencilState->Bind(context.Get());

		if (m_blendState)
			m_blendState->Bind(context.Get());

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

	bool RenderTechnique::IsEnabled() {
		return m_enabled;
	}

	void RenderTechnique::Disable() {
		m_enabled = false;
	}

	void RenderTechnique::Enable() {
		m_enabled = true;
	}

	RenderTechnique::RenderTechnique(RenderTechnique&& other) noexcept
		: m_mesh(eastl::move(other.m_mesh)),
		m_vertexShader(eastl::move(other.m_vertexShader)),
		m_pixelShader(eastl::move(other.m_pixelShader)),
		m_texture(eastl::move(other.m_texture)),
		m_textureSampler(eastl::move(other.m_textureSampler)),
		m_blendState(eastl::move(other.m_blendState)),
		m_rasterizer(eastl::move(other.m_rasterizer)),
		m_depthStencilState(eastl::move(other.m_depthStencilState)),
		m_bindables(eastl::move(other.m_bindables)),
		m_techniqueTag(eastl::move(other.m_techniqueTag)),
		m_assignedTransform(other.m_assignedTransform),
		m_enabled(other.m_enabled)
	{
		// leave other in a valid state
		other.m_assignedTransform = nullptr;
		other.m_enabled = false;
	}

	RenderTechnique& RenderTechnique::operator=(RenderTechnique&& other) noexcept
	{
		if (this != &other) {
			m_mesh = eastl::move(other.m_mesh);
			m_vertexShader = eastl::move(other.m_vertexShader);
			m_pixelShader = eastl::move(other.m_pixelShader);
			m_texture = eastl::move(other.m_texture);
			m_textureSampler = eastl::move(other.m_textureSampler);

			m_blendState = eastl::move(other.m_blendState);
			m_rasterizer = eastl::move(other.m_rasterizer);
			m_depthStencilState = eastl::move(other.m_depthStencilState);

			m_bindables = eastl::move(other.m_bindables);
			m_techniqueTag = eastl::move(other.m_techniqueTag);
			m_assignedTransform = other.m_assignedTransform;
			m_enabled = other.m_enabled;

			// reset other
			other.m_assignedTransform = nullptr;
			other.m_enabled = false;
		}
		return *this;
	}
}