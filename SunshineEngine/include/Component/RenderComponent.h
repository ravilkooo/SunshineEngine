#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>

#include <Graphics/RenderTechnique.h>
#include <Bindable/Bindable.h>

#include "Component.h"

class RenderComponent :
    public Component
{
public:
    RenderComponent() = default;
    ~RenderComponent() = default;

    RenderComponent(const RenderComponent&) = delete;
    RenderComponent& operator=(const RenderComponent&) = delete;

    RenderComponent(RenderComponent&&) noexcept = default;
    RenderComponent& operator=(RenderComponent&&) noexcept = default;

    //void Render(ID3D11DeviceContext* context);

    bool HasTechnique(eastl::string technique);

    virtual void DrawTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept;

    void PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    eastl::map<eastl::string, eastl::unique_ptr<RenderTechnique>> techniques;

    const std::type_info& getType() const override {
        return typeid(RenderComponent);
    }

};

