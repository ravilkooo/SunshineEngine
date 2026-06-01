#pragma once
#include <d3d11.h>

#include "RenderTechnique.h"
#include <Graphics/Renderer/DeferredRenderer.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/Bindable/Sampler.h>
#include <Graphics/Bindable/ConstantBuffer.h>
#include <Graphics/Bindable/BlendState.h>
#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/DepthStencilState.h>

#include <Graphics/Utils/Camera.h>
#include <Graphics/Lighting/LightData.h>

namespace SE_G {
    enum class LightPosition {
        INSIDE, INTERSECT_FAR_PLANE, OUTSIDE, FILL, BEHIND_NEAR_PLANE
    };

	// ------- static data -------
	class LightStaticData
	{
	public:
		static bool s_staticDataInitializated;
		static eastl::shared_ptr<Bind::DepthStencilState> depthCompLess;
		static eastl::shared_ptr<Bind::DepthStencilState> depthCompGreater;

		static eastl::shared_ptr<Bind::Rasterizer> rastCullNone;
		static eastl::shared_ptr<Bind::Rasterizer> rastCullBack;
		static eastl::shared_ptr<Bind::Rasterizer> rastCullFront;

		static void InitStaticData(ID3D11Device* device);
	};

	
	// ------- ---------- -------



    template <class T>
    class LightTechnique :
        public RenderTechnique
    {
    public:
        eastl::shared_ptr<T> m_lightData;
        eastl::shared_ptr<Bind::VertexConstantBuffer<T>> m_lightDataVertexCBuffer;
        eastl::shared_ptr<Bind::PixelConstantBuffer<T>> m_lightDataPixelCBuffer;
		DeferredRenderer* m_renderer = nullptr;

        LightTechnique(DeferredRenderer* renderer, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<T> lightData)
			: RenderTechnique(renderer->GetDevice(), assignedTransform, technique)
		{
			m_renderer = renderer;
			auto device = m_renderer->GetDevice();
			if (!LightStaticData::s_staticDataInitializated)
			{
				LightStaticData::InitStaticData(device);
			}

			D3D11_BLEND_DESC blendDesc = {};
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			m_blendState = eastl::make_unique<Bind::BlendState>(device, blendDesc);

			m_lightData = lightData;
			m_lightDataPixelCBuffer =
				eastl::make_shared<Bind::PixelConstantBuffer<T>>(
					device,
					*(lightData),
					2u
				);

			m_lightDataVertexCBuffer =
				eastl::make_shared<Bind::VertexConstantBuffer<T>>(
					device,
					*(lightData),
					2u
				);
		}

		virtual ~LightTechnique() = default;

        virtual void BindAll(ID3D11DeviceContext* context) override
		{
			for (size_t i = 0; i < m_bindables.size(); i++)
			{
				m_bindables[i]->Bind(context);
			}

			if (m_vertexShader) {
				m_vertexShader->Bind(context);
			}

			if (m_pixelShader) {
				m_pixelShader->Bind(context);
			}

			if (m_texture) {
				m_texture->Bind(context);
			}

			if (m_textureSampler) {
				m_textureSampler->Bind(context);
			}

			if (m_lightDataPixelCBuffer) {
				m_lightDataPixelCBuffer->Bind(context);
			}

			if (m_lightDataVertexCBuffer) {
				m_lightDataVertexCBuffer->Bind(context);
			}

			if (m_blendState)
				m_blendState->Bind(context);

			if (m_mesh)
				m_mesh->Bind(context);

			LightPosition lightPos = GetLightPositionInFrustum(m_renderer->GetMainCamera().get());
			// Choose rasterizer
			ChooseRasterizer(context, lightPos);
			// Choose depthState
			ChooseDepthStencilState(context, lightPos);

			// Bind rasterizer
			if (m_rasterizer)
				m_rasterizer->Bind(context);

			// Bind depthState
			if (m_depthStencilState)
				m_depthStencilState->Bind(context);
		}

        // Update right before draw
        // Need camera?
        virtual LightPosition GetLightPositionInFrustum(Camera* camera) { return LightPosition::FILL; };
        virtual bool IsFrustumInsideOfLight(Camera* camera) { return true; };

        virtual void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) = 0;
        virtual void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) = 0;
    };
}