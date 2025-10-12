#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <string>

#include <memory>

#include "GraphicsResources/Mesh.h"
#include "GraphicsResources/VertexShader.h"
#include "GraphicsResources/PixelShader.h"
#include "GraphicsResources/Texture.h"


namespace Bind {
    class Bindable;
}

class RenderTechnique
{
public:
    RenderTechnique(std::string technic);

    void AddBind(Bind::Bindable* bind);
    std::vector<Bind::Bindable*> bindables;
    
    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    void DrawTechnique(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Bind::Texture> texture;
    std::shared_ptr<Bind::Sampler> textureSampler;
    std::shared_ptr<Bind::VertexShader> vertexShader;
    std::shared_ptr<Bind::PixelShader> pixelShader;

    std::string GetTechnique();

protected:
    std::string techniqueTag;
};

