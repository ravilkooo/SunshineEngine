#include <ResourceManager/Enums/ResourceType.h>

const char* SunshineResource::ResourceTypeToString(ResourceType type)
{
    static const char* typeNames[] = {
        "TEXTURE", "MESH", "MATERIAL",
        "SHADER", "SKELETON", "ANIMATION", "UNKNOWN"
    };

    size_t index = static_cast<size_t>(type);
    return (index < static_cast<size_t>(ResourceType::COUNT))
        ? typeNames[index]
        : typeNames[static_cast<size_t>(ResourceType::COUNT)];
}

