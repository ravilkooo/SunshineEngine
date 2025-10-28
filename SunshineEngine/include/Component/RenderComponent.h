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

    bool HasTechnique(eastl::string technique);

    void PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    eastl::map<eastl::string, eastl::unique_ptr<RenderTechnique>> techniques;

    const std::type_info& getType() const override {
        return typeid(RenderComponent);
    }

};

// Methods of RenderComponent to expose in Lua bindings
#ifndef RENDERCOMPONENT_LUA_METHODS_APPLY
#define RENDERCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("hasTechnique", [](RenderComponent* self, const char* technique) -> bool { \
        return self->HasTechnique(eastl::string(technique));                       \
    })
#endif