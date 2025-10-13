#pragma once

#include <wrl.h>
#include <d3d11.h>

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
    RenderTechnique(eastl::string technique);
    ~RenderTechnique() = default;

    void AddBind(Bind::Bindable* bind);
    eastl::vector<Bind::Bindable*> bindables;

    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    eastl::shared_ptr<Mesh> mesh;
    eastl::shared_ptr<Bind::Texture> texture;
    eastl::shared_ptr<Bind::Sampler> textureSampler;
    eastl::shared_ptr<Bind::VertexShader> vertexShader;
    eastl::shared_ptr<Bind::PixelShader> pixelShader;

    eastl::string GetTechnique();

protected:
    eastl::string techniqueTag;
};

