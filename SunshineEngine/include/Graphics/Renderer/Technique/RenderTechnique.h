#pragma once

#include <wrl.h>
#include <d3d11.h>

#include <Graphics/Bindable/Rasterizer.h>
#include <Graphics/Bindable/DepthStencilState.h>
#include <Graphics/Bindable/BlendState.h>

#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/GraphicsResources/VertexShader.h>
#include <Graphics/GraphicsResources/PixelShader.h>
#include <Graphics/GraphicsResources/Texture.h>

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>

class TransformComponent;

namespace SE_G {
    namespace Bind {
        class Bindable;
    }

    class RenderTechnique
    {
    public:
        RenderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,  eastl::string technique);
        virtual ~RenderTechnique();

        void AddBind(eastl::shared_ptr<Bind::Bindable> bind);

        virtual void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        virtual void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        virtual void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        virtual eastl::string GetTechniqueTag();

        // Resources
        eastl::shared_ptr<Mesh> m_mesh;
        eastl::shared_ptr<Bind::VertexShader> m_vertexShader;
        eastl::shared_ptr<Bind::PixelShader> m_pixelShader;
        eastl::shared_ptr<Bind::Texture> m_texture;

        // Extra (has default values)
        eastl::shared_ptr<Bind::Sampler> m_textureSampler;
        eastl::shared_ptr<Bind::BlendState> blendState;
        eastl::shared_ptr<Bind::Rasterizer> rasterizer;
        eastl::shared_ptr<Bind::DepthStencilState> depthStencilState;

        eastl::vector<eastl::shared_ptr<Bind::Bindable>> bindables;

        eastl::string m_techniqueTag;

        TransformComponent* m_assignedTransform;
    };
}
