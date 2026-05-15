#include <Component/MeshComponent.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>

#include <Graphics/Renderer/Technique/GPassTechnique.h>

#include <Graphics/GraphicsResources/Mesh.h>
#include <Graphics/GraphicsResources/Texture.h>
#include <Graphics/Bindable/Sampler.h>

#include <Utils/StringUtils.h>

MeshData::MeshData(MeshData&& other) noexcept
    : m_mesh(eastl::move(other.m_mesh)),
    m_texture(eastl::move(other.m_texture)),
    m_textureSampler(eastl::move(other.m_textureSampler))
{
}

MeshData& MeshData::operator=(MeshData&& other) noexcept
{
    if (this != &other) {
        m_mesh = eastl::move(other.m_mesh);
        m_texture = eastl::move(other.m_texture);
        m_textureSampler = eastl::move(other.m_textureSampler);
    }
    return *this;
}

MeshComponent::MeshComponent(RenderComponent* rc, TransformComponent* tc,
    SE::UUID uuid, AssetPath meshPath)
{
    m_meshData = eastl::make_shared<MeshData>();

    m_meshData->m_mesh = eastl::make_shared<SE_G::Mesh>(rc->GetDevice(), meshPath);
    m_meshData->m_texture = eastl::make_shared<SE_G::Bind::Texture>(
        rc->GetDevice(),
        SE_G::Colors::UnloadedTextureColor, 0u,
        SE_G::Bind::PipelineStage::PIXEL_SHADER);
    m_meshData->m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
        rc->GetDevice(),
        SE_G::Bind::SamplerPreset::Wrap);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        rc->GetDevice(), tc, "GPass", uuid);
    m_gBufferTech = static_cast<SE_G::GPassTechnique*>(rc->AddTechnique(eastl::move(gBufferTech)));

    m_gBufferTech->InitByMeshData(m_meshData);
}

MeshComponent::MeshComponent(RenderComponent* rc, TransformComponent* tc,
    SE::UUID uuid, eastl::shared_ptr<SE_G::Mesh> mesh)
{
    m_meshData = eastl::make_shared<MeshData>();

    m_meshData->m_mesh = mesh;
    m_meshData->m_texture = eastl::make_shared<SE_G::Bind::Texture>(
        rc->GetDevice(),
        SE_G::Colors::UnloadedTextureColor, 0u,
        SE_G::Bind::PipelineStage::PIXEL_SHADER);
    m_meshData->m_textureSampler = eastl::make_shared<SE_G::Bind::Sampler>(
        rc->GetDevice(),
        SE_G::Bind::SamplerPreset::Wrap);

    auto gBufferTech = eastl::make_unique<SE_G::GPassTechnique>(
        rc->GetDevice(), tc, "GPass", uuid);
    m_gBufferTech = static_cast<SE_G::GPassTechnique*>(rc->AddTechnique(eastl::move(gBufferTech)));

    m_gBufferTech->InitByMeshData(m_meshData);
}

MeshComponent::~MeshComponent()
{
    /*
    if (m_gBufferTech)
    {
        if (m_gBufferTech->m_assignedTransform)
        {
            m_gBufferTech->m_assignedTransform->m_localPosition = DXSM::Vector3::Zero;
            m_gBufferTech->m_assignedTransform->m_localRotation = DXSM::Vector3::Zero;
            m_gBufferTech->m_assignedTransform->m_localScaleFactor = DXSM::Vector3::One;
        }
    }
    */
}

MeshComponent::MeshComponent(MeshComponent&& other) noexcept
    : m_meshData(eastl::move(other.m_meshData))
{
}

MeshComponent& MeshComponent::operator=(MeshComponent&& other) noexcept
{
    if (this != &other) {
        m_meshData = eastl::move(other.m_meshData);
    }
    return *this;
}

D3D11_CULL_MODE MeshComponent::GetCullMode()
{
    return m_cullMode;
};

void MeshComponent::SetCullMode(D3D11_CULL_MODE cullMode)
{
    m_cullMode = cullMode;

    D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rastDesc.CullMode = m_cullMode;
    rastDesc.FillMode = m_fillMode;
    m_gBufferTech->SetRasterizer(rastDesc);
}

/*
void MeshComponent::Bind(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    m_mesh->Bind(context.Get());
    m_texture->Bind(context.Get());
    m_textureSampler->Bind(context.Get());
}
*/

MeshComponent_Info::MeshComponent_Info(
    RenderComponent_Info* rc_info, TransformComponent_Info* tc_info,
    SE::UUID uuid, AssetPath meshPath)
{
    m_assignedComponent = eastl::make_unique<MeshComponent>(
        rc_info->m_assignedComponent.get(),
        tc_info->m_assignedComponent.get(), uuid, meshPath);
    
    rc_info->AddTechnique_Info(rc_info->m_assignedComponent->GetTechnique("GPass"));

    m_rc_info = rc_info;
}

MeshComponent_Info::MeshComponent_Info(
    RenderComponent_Info* rc_info, TransformComponent_Info* tc_info,
    SE::UUID uuid, eastl::shared_ptr<SE_G::Mesh> mesh)
{
    m_assignedComponent = eastl::make_unique<MeshComponent>(
        rc_info->m_assignedComponent.get(),
        tc_info->m_assignedComponent.get(),
        uuid, mesh);

    rc_info->AddTechnique_Info(rc_info->m_assignedComponent->GetTechnique("GPass"));

    m_rc_info = rc_info;
}

MeshComponent_Info::~MeshComponent_Info()
{
    /*
    if (m_assignedComponent->m_gBufferTech && m_assignedComponent->m_gBufferTech->m_assignedTransform)
    {
        m_assignedComponent->m_gBufferTech->m_assignedTransform->m_localPosition = DXSM::Vector3::Zero;
        m_assignedComponent->m_gBufferTech->m_assignedTransform->m_localRotation = DXSM::Vector3::Zero;
        m_assignedComponent->m_gBufferTech->m_assignedTransform->m_localScaleFactor = DXSM::Vector3::One;
    }
    */
    m_rc_info->RemoveTechnique("GPass");
}

MeshComponent_Info::MeshComponent_Info(MeshComponent_Info&& other) noexcept
    : m_assignedComponent(eastl::move(other.m_assignedComponent)),
      m_rc_info(other.m_rc_info)
{
    // Steal the rc pointer without removing the technique on the source
    other.m_rc_info = nullptr;
}

MeshComponent_Info& MeshComponent_Info::operator=(MeshComponent_Info&& other) noexcept
{
    if (this != &other) {
        // If this currently manages a technique, remove it before overwriting
        if (m_rc_info) {
            m_rc_info->RemoveTechnique("GPass");
        }

        m_assignedComponent = eastl::move(other.m_assignedComponent);
        m_rc_info = other.m_rc_info;
        other.m_rc_info = nullptr;
    }
    return *this;
}

