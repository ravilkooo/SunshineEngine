#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>

#include <Graphics/Renderer/Technique/RenderTechnique.h>
#include <Graphics/Bindable/Bindable.h>

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

    eastl::map<eastl::string, eastl::unique_ptr<SE_G::RenderTechnique>> techniques;

    const std::type_info& getType() const override {
        return typeid(RenderComponent);
    }

};

class RenderComponent_Info : public Component_Info
{
public:
    ~RenderComponent_Info();

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
            m_hasGPassMesh = true;
            m_gPassTech = dynamic_cast<SE_G::GPassTechnique*>(tech.get());
        }

        techniques.insert(tech->GetTechniqueTag());
        m_assignedComponent->AddTechnique(eastl::move(tech));

    }
    
    bool HasTechnique(eastl::string technique) {
        return (techniques.find(technique) != techniques.end());
    }

    bool HasGPassMesh();
    void SetMesh(const eastl::string& filePath);
    void SetMesh(eastl::shared_ptr<SE_G::Mesh> newMesh);

    void SetMeshTexture(const eastl::wstring& filePath,
        SE_G::Bind::SamplerPreset samplerPreset = SE_G::Bind::SamplerPreset::Wrap);
    eastl::string GetCurrentMeshPath() const;
    eastl::wstring GetCurrentTexturePath() const;
    SE_G::Bind::SamplerPreset GetCurrentTextureSampler() const;
    
    eastl::unordered_set<eastl::string> techniques;
    eastl::unique_ptr<RenderComponent> m_assignedComponent;

    SE_G::RenderTechnique* m_selectionTechnique;
    
    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

private:
    SE_G::GPassTechnique* m_gPassTech;
    bool m_hasGPassMesh = false;
};

/*
// Methods of RenderComponent to expose in Lua bindings
#ifndef RENDERCOMPONENT_LUA_METHODS_APPLY
#define RENDERCOMPONENT_LUA_METHODS_APPLY(FM) \
    FM("hasTechnique", [](RenderComponent* self, const char* technique) -> bool { \
        return self->HasTechnique(eastl::string(technique));                       \
    })
#endif