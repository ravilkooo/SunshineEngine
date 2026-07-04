#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <EASTL/string.h>
#include <EASTL/map.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_set.h>

#include <Component/Component.h>
#include <Utils/UUID.h>

namespace SE_G {
    class DeferredRenderer;
    class GPassTechnique;
    class RenderTechnique;
}

//class GameObject_Info;

class RenderComponent :
    public Component
{

    friend class RenderComponent_Info;
public:

    RenderComponent() = default;
    RenderComponent(SE::UUID uuid, SE_G::DeferredRenderer* renderSystem)
        : m_renderSystem(renderSystem), m_objectUUID(uuid) {}
    ~RenderComponent();

    RenderComponent(const RenderComponent&) = delete;
    RenderComponent& operator=(const RenderComponent&) = delete;

    RenderComponent(RenderComponent&&) noexcept = default;
    RenderComponent& operator=(RenderComponent&&) noexcept = default;

    SE_G::RenderTechnique* AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique>);
    SE_G::RenderTechnique* GetTechnique(eastl::string technique);
    void RemoveTechnique(eastl::string technique);

    const std::type_info& getType() const override {
        return typeid(RenderComponent);
    }
    
    static const SE::ComponentType s_componentType = SE::ComponentType::RENDER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    bool GetVisibility();
    void SetVisibility(bool newVisibilty);
    void ToggleVisibility();

    void FromJson(const json& j) override;

private:
    bool m_isVisible = true;
    bool m_isTransparent = false;
    SE_G::DeferredRenderer* m_renderSystem;
    SE::UUID m_objectUUID;
};

class RenderComponent_Info : public Component_Info
{
public:
    RenderComponent_Info(SE::UUID uuid, SE_G::DeferredRenderer* renderSystem);
    ~RenderComponent_Info();

    static const SE::ComponentType s_componentType = SE::ComponentType::RENDER;
    const SE::ComponentType ComponentType() const override {
        return s_componentType;
    }

    const std::type_info& getType() const override {
        return typeid(RenderComponent_Info);
    }

    bool IsAssigned() override { return true; }

    // Just add only in Info component
    void AddTechnique_Info(SE_G::RenderTechnique* tech);

    // Also add in Info component
    SE_G::RenderTechnique* AddTechnique(eastl::unique_ptr<SE_G::RenderTechnique> tech);
    
    bool HasTechnique(eastl::string technique);

    void RemoveTechnique(eastl::string technique);

    bool HasGPassMesh();

    eastl::unordered_set<eastl::string> techniques;
    eastl::unique_ptr<RenderComponent> m_assignedComponent;

    SE_G::RenderTechnique* m_selectionTechnique;
    
    // Serialization
    json ToJson() const override;
    void FromJson(const json& j) override;

    ID3D11Device* GetDevice() { return m_assignedComponent->GetDevice(); }
    ID3D11DeviceContext* GetDeviceContext() { return m_assignedComponent->GetDeviceContext(); }

    bool GetVisibility();
    void SetVisibility(bool newVisibilty);
    void ToggleVisibility();

    bool m_isVisible = true;
    bool m_isTransparent = false;

private:
    SE_G::GPassTechnique* m_gPassTech;
    bool m_hasGPassMesh = false;
};
