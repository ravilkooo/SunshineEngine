#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_set.h>

#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Bindable/Bindable.h>

#include <Component/Component.h>
#include <Utils/UUID.h>

namespace SE_G {
    class DeferredRenderer;
}

class RenderComponent :
    public Component
{

    friend class RenderComponent_Info;
public:

    RenderComponent() = default;
    RenderComponent(SE_G::DeferredRenderer* renderSystem) : m_renderSystem(renderSystem) {}
    ~RenderComponent() = default;

    RenderComponent(const RenderComponent&) = delete;
    RenderComponent& operator=(const RenderComponent&) = delete;

    RenderComponent(RenderComponent&&) noexcept = default;
    RenderComponent& operator=(RenderComponent&&) noexcept = default;

    void AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique>);

    /*
    bool HasTechnique(eastl::string technique);

    void PassTechnique(eastl::string technique, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    */

    const std::type_info& getType() const override {
        return typeid(RenderComponent);
    }
    
    static const SE::ComponentType s_componentType = SE::ComponentType::RENDER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    SE_G::DeferredRenderer* m_renderSystem;
    Sunshine::UUID m_objectUUID;
};

class RenderComponent_Info : public Component_Info
{
public:
    static const SE::ComponentType s_componentType = SE::ComponentType::RENDER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(RenderComponent_Info);
    }

    bool IsAssigned() override { return true; }

    void AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> tech)
    {
        if (tech->GetTechniqueTag() == "IconPass") {
            m_selectionTechnique = tech.get();
        }
        else if (tech->GetTechniqueTag() == "GPass") {
            m_selectionTechnique = tech.get();
        }

        techniques.insert(tech->GetTechniqueTag());
        m_assignedComponent->AddTechnique(eastl::move(tech));

    }
    
    bool HasTechnique(eastl::string technique) {
        return (techniques.find(technique) != techniques.end());
    }
    
    eastl::unordered_set<eastl::string> techniques;
    eastl::shared_ptr<RenderComponent> m_assignedComponent;

    SE_G::RenderTechnique* m_selectionTechnique;
    
    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;
};

/*
// Methods of RenderComponent to expose in Lua bindings
#ifndef RENDERCOMPONENT_LUA_METHODS_APPLY
#define RENDERCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("hasTechnique", [](RenderComponent* self, const char* technique) -> bool { \
        return self->HasTechnique(eastl::string(technique));                       \
    })
#endif
*/