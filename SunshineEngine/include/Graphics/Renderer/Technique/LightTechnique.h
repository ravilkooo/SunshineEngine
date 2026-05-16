#pragma once
#include "RenderTechnique.h"

#include <Graphics/Bindable/ConstantBuffer.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/GraphicsResources/Mesh.h>																																						.h>
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
        eastl::shared_ptr<Camera> m_camera;

        LightTechnique(ID3D11Device* device, TransformComponent* assignedTransform, eastl::string technique,
            eastl::shared_ptr<Camera> camera,
            eastl::shared_ptr<T> lightData)
			: RenderTechnique(device, assignedTransform, technique)
		{

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

			m_camera = camera;
		}

		virtual ~LightTechnique() = default;

        virtual void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override
		{
			for (size_t i = 0; i < m_bindables.size(); i++)
			{
				m_bindables[i]->Bind(context.Get());
			}

			if (m_vertexShader) {
				m_vertexShader->Bind(context.Get());
			}

			if (m_pixelShader) {
				m_pixelShader->Bind(context.Get());
			}

			if (m_texture) {
				m_texture->Bind(context.Get());
			}

			if (m_textureSampler) {
				m_textureSampler->Bind(context.Get());
			}

			if (m_lightDataPixelCBuffer) {
				m_lightDataPixelCBuffer->Bind(context.Get());
			}

			if (m_lightDataVertexCBuffer) {
				m_lightDataVertexCBuffer->Bind(context.Get());
			}

			if (m_blendState)
				m_blendState->Bind(context.Get());

			if (m_mesh)
				m_mesh->Bind(context.Get());

			LightPosition lightPos = GetLightPositionInFrustum();
			// Choose rasterizer
			ChooseRasterizer(context.Get(), lightPos);
			// Choose depthState
			ChooseDepthStencilState(context.Get(), lightPos);

			// Bind rasterizer
			if (m_rasterizer)
				m_rasterizer->Bind(context.Get());

			// Bind depthState
			if (m_depthStencilState)
				m_depthStencilState->Bind(context.Get());
		}

        // Update right before draw
        // Need camera?
        virtual LightPosition GetLightPositionInFrustum() { return LightPosition::FILL; };
        virtual bool IsFrustumInsideOfLight() { return true; };

        virtual void ChooseDepthStencilState(ID3D11DeviceContext* context, LightPosition lightPos) = 0;
        virtual void ChooseRasterizer(ID3D11DeviceContext* context, LightPosition lightPos) = 0;
    };
    /*
    template class LightTechnique<AmbientLightData>;
    template void LightTechnique<AmbientLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

    template class LightTechnique<DirectionalLightData>;
    template void LightTechnique<DirectionalLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

    template class LightTechnique<PointLightData>;
    template void LightTechnique<PointLightData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);

    template class LightTechnique<SkyBoxData>;
    template void LightTechnique<SkyBoxData>::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);
    */
}