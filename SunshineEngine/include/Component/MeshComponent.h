#pragma once

#include "Component.h"
#include <Graphics/GraphicsResources/Mesh.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

class MeshComponent : public Component
{
public:
    MeshComponent() = default;
    MeshComponent(eastl::shared_ptr<SE_G::Mesh> mesh) : m_mesh(mesh) {}

    const std::type_info& getType() const override { return typeid(MeshComponent); }
    static const SE::ComponentType s_componentType = SE::ComponentType::MESH;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    eastl::shared_ptr<SE_G::Mesh> GetMesh() const { return m_mesh; }
    void SetMesh(eastl::shared_ptr<SE_G::Mesh> mesh) { m_mesh = mesh; }

private:
    eastl::shared_ptr<SE_G::Mesh> m_mesh;
};

class MeshComponent_Info : public Component_Info
{
public:
    MeshComponent_Info() = default;
    ~MeshComponent_Info() override = default;

    const std::type_info& getType() const override { return typeid(MeshComponent_Info); }
    static const SE::ComponentType s_componentType = SE::ComponentType::MESH;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    bool IsAssigned() override { return m_assignedComponent != nullptr; }

    eastl::unique_ptr<MeshComponent> m_assignedComponent;
};
