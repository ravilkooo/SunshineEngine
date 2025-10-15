#pragma once

#include <wrl.h>
#include <d3d11.h>

#include <Bindable/Rasterizer.h>
#include <Bindable/DepthStencilState.h>
#include <Bindable/BlendState.h>

#include <GraphicsResources/Mesh.h>
#include <GraphicsResources/VertexShader.h>
#include <GraphicsResources/PixelShader.h>
#include <GraphicsResources/Texture.h>

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>


namespace Bind {
    class Bindable;
}

class RenderTechnique
{
public:
    RenderTechnique(ID3D11Device* device, eastl::string technique);
    virtual ~RenderTechnique() = default;

    void AddBind(Bind::Bindable* bind);
    eastl::vector<Bind::Bindable*> bindables;

    virtual void Pass(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    virtual void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    virtual void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    // Resources
    eastl::shared_ptr<Mesh> mesh;
    eastl::shared_ptr<Bind::VertexShader> vertexShader;
    eastl::shared_ptr<Bind::PixelShader> pixelShader;
    eastl::shared_ptr<Bind::Texture> texture;

    // Extra (has default values)
    eastl::shared_ptr<Bind::Sampler> textureSampler;
    eastl::shared_ptr<Bind::BlendState> blendState;
    eastl::shared_ptr<Bind::Rasterizer> rasterizer;
    eastl::shared_ptr<Bind::DepthStencilState> depthStencilState;

    virtual eastl::string GetTechnique();

    eastl::string techniqueTag;
};

