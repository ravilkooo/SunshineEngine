#pragma once

#include <wrl.h>
#include <d3d11.h>

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>

// forward-declare heavy graphics types to reduce header dependencies
namespace SE_G {
    class Mesh;
    namespace Bind {
        class VertexShader;
        class PixelShader;
        class Texture;
        class Sampler;
        class BlendState;
        class Rasterizer;
        class DepthStencilState;
        class Bindable;
    }
}

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
        // move
        RenderTechnique(RenderTechnique&& other) noexcept;
        RenderTechnique& operator=(RenderTechnique&& other) noexcept;
        virtual ~RenderTechnique();

        void AddBind(eastl::shared_ptr<Bind::Bindable> bind);

        virtual void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        virtual void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
        virtual void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

        virtual eastl::string GetTechniqueTag();

        bool IsEnabled();
        void Disable();
        void Enable();

        // Resources
        eastl::shared_ptr<SE_G::Mesh> m_mesh;
        eastl::shared_ptr<SE_G::Bind::VertexShader> m_vertexShader;
        eastl::shared_ptr<SE_G::Bind::PixelShader> m_pixelShader;
        eastl::shared_ptr<SE_G::Bind::Texture> m_texture;
        eastl::shared_ptr<SE_G::Bind::Sampler> m_textureSampler;

        // Extra (has default values)
        eastl::unique_ptr<SE_G::Bind::BlendState> m_blendState;
        eastl::unique_ptr<SE_G::Bind::Rasterizer> m_rasterizer;
        eastl::unique_ptr<SE_G::Bind::DepthStencilState> m_depthStencilState;

        eastl::vector<eastl::shared_ptr<SE_G::Bind::Bindable>> m_bindables;

        eastl::string m_techniqueTag;

        // TO-DO: make weak ptr
        TransformComponent* m_assignedTransform;

    private:
        bool m_enabled = true;
    };
}
