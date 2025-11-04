#pragma once
#include "Enums/ResourceType.h"
#include <EASTL/string.h>
#include "ResourceLoader/IResourceLoader.h"
#include <EASTL/hash_map.h>
#include <EASTL/unique_ptr.h>

class ResourceLoaderFactory
{
public:
    static void RegisterLoader(ResourceType type,
        eastl::unique_ptr<IResourceLoader> loader);

    static IResourceLoader* GetLoader(ResourceType type);
    static IResourceLoader* GetLoaderForFile(const eastl::string& path);

private:
    static eastl::hash_map<int, eastl::unique_ptr<IResourceLoader>> m_Loaders;
};
