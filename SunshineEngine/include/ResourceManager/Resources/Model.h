#pragma once
#include <ResourceManager/Enums/ResourceType.h>
#include <ResourceManager/ResourceHandle.h>
#include <ResourceManager/IResource.h>
#include <EASTL/vector.h>

class Model : public IResource
{
public:
    SunshineResource::ResourceType GetType() const override { return SunshineResource::ResourceType::MESH; }

    // Эти ресурсы должны быть загружены до модели
    eastl::vector<ResourceGUID> GetMeshes() const { return m_Meshes; }
    eastl::vector<ResourceGUID> GetMaterials() const { return m_Materials; }
    eastl::vector<ResourceGUID> GetTextures() const { return m_Textures; }
    ResourceGUID GetSkeleton() const { return m_Skeleton; }
    eastl::vector<ResourceGUID> GetAnimations() const { return m_Animations; }


    eastl::vector<ResourceGUID> m_Textures;
    eastl::vector<ResourceGUID> m_Meshes;
    eastl::vector<ResourceGUID> m_Materials;
    eastl::vector<ResourceGUID> m_Shaders;
    ResourceGUID m_Skeleton;
    eastl::vector<ResourceGUID> m_Animations;

    // Inherited via IResource
    ResourceGUID GetGUID() const override;
    size_t GetSizeInMemory() const override;
};

