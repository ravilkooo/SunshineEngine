#pragma once



#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <string>

namespace Bind {
    class Bindable;
}

class RenderTechnique
{
public:
    RenderTechnique(std::string technique);
    void AddBind(Bind::Bindable* bind);
    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    std::vector<Bind::Bindable*> bindables;

    std::string GetTechnique();

protected:
    std::string techniqueTag;
};

