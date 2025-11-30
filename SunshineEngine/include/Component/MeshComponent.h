#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/string.h>

#include "Component.h"
// forward declare heavy graphics types used by pointer/shared_ptr - include actual headers in .cpp
namespace SE_G { class Mesh; namespace Bind { class Texture; class Sampler; } }

// forward-declare D3D device to avoid pulling d3d headers into this header
struct ID3D11Device;

#include <Utils/UUID.h>

class RenderComponent;
class RenderComponent_Info;
class TransformComponent;
class TransformComponent_Info;

namespace SE_G
{
    class GPassTechnique;
}

class MeshData
{
public:
    MeshData() {};

    // move
    MeshData(MeshData&& other) noexcept;
    MeshData& operator=(MeshData&& other) noexcept;

    eastl::shared_ptr<SE_G::Mesh> m_mesh;
    eastl::shared_ptr<SE_G::Bind::Texture> m_texture;
    eastl::shared_ptr<SE_G::Bind::Sampler> m_textureSampler;
};

class MeshComponent : public Component
{
public:
    MeshComponent() = default;
    MeshComponent(RenderComponent* rc, TransformComponent* tc, SE::UUID uuid, const eastl::string& meshPath);
    MeshComponent(RenderComponent* rc, TransformComponent* tc, SE::UUID uuid, eastl::shared_ptr<SE_G::Mesh> mesh);
    ~MeshComponent();

    // move
    MeshComponent(MeshComponent&& other) noexcept;
    MeshComponent& operator=(MeshComponent&& other) noexcept;

    //void Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    const std::type_info& getType() const override { return typeid(MeshComponent); }
    static const SE::ComponentType s_componentType = SE::ComponentType::MESH;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    eastl::shared_ptr<SE_G::Mesh> GetMesh() const { return m_meshData->m_mesh; }
    void SetMesh(eastl::shared_ptr<SE_G::Mesh> mesh) { m_meshData->m_mesh = mesh; }

    eastl::shared_ptr<SE_G::Bind::Texture> GetTexture() { return m_meshData->m_texture; }
    void SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> texture) { m_meshData->m_texture = texture; }

    void SetTextureSamplerPreset(eastl::shared_ptr<SE_G::Bind::Sampler> sampler) { m_meshData->m_textureSampler = sampler; }
    eastl::shared_ptr<SE_G::Bind::Sampler> GetTextureSamplerPreset() { return m_meshData->m_textureSampler; }

    void FromJson(const json& j, ID3D11Device* device);

private:
    eastl::shared_ptr<MeshData> m_meshData;
    SE_G::GPassTechnique* m_gBufferTech;
};

class MeshComponent_Info : public Component_Info
{
public:
    MeshComponent_Info() = default;
    MeshComponent_Info(RenderComponent_Info* rc, TransformComponent_Info* tc, SE::UUID uuid, const eastl::string& path);
    MeshComponent_Info(RenderComponent_Info* rc, TransformComponent_Info* tc, SE::UUID uuid, eastl::shared_ptr<SE_G::Mesh> mesh);
    ~MeshComponent_Info() override;

    // move
    MeshComponent_Info(MeshComponent_Info&& other) noexcept;
    MeshComponent_Info& operator=(MeshComponent_Info&& other) noexcept;

    const std::type_info& getType() const override { return typeid(MeshComponent_Info); }
    static const SE::ComponentType s_componentType = SE::ComponentType::MESH;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    eastl::shared_ptr<SE_G::Mesh> GetMesh() const { return m_assignedComponent->GetMesh(); }
    void SetMesh(eastl::shared_ptr<SE_G::Mesh> mesh) { m_assignedComponent->SetMesh(mesh); }

    eastl::shared_ptr<SE_G::Bind::Texture> GetTexture() { return m_assignedComponent->GetTexture(); }
    void SetTexture(eastl::shared_ptr<SE_G::Bind::Texture> texture) { m_assignedComponent->SetTexture(texture); }

    eastl::shared_ptr<SE_G::Bind::Sampler> GetTextureSamplerPreset() { return m_assignedComponent->GetTextureSamplerPreset(); }
    void SetTextureSamplerPreset(eastl::shared_ptr<SE_G::Bind::Sampler> sampler) { m_assignedComponent->SetTextureSamplerPreset(sampler); }

    bool IsAssigned() override { return m_assignedComponent != nullptr; }

    eastl::unique_ptr<MeshComponent> m_assignedComponent;

    // Serialization
    json ToJson() const override;
    void FromJson(const json& j, ID3D11Device* device);

    RenderComponent_Info* m_rc_info;
};
