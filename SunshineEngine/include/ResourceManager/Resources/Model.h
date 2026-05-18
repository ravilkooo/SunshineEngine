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
    eastl::vector<ResourceGUID> GetTextures() const { return m_Textures; }


    eastl::vector<ResourceGUID> m_Textures;
    eastl::vector<ResourceGUID> m_Meshes;
    eastl::vector<ResourceGUID> m_Shaders;

    // Inherited via IResource
    ResourceGUID GetGUID() const override;
    size_t GetSizeInMemory() const override;
};

