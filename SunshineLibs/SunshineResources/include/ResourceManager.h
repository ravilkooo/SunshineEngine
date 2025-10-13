#pragma once

#include "Resource.h"

#include <filesystem>
#include <unordered_map>


class ResourceManager {
public:
    static ResourceManager& Instance() {
        static ResourceManager inst;
        return inst;
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> Load(const std::string& id, Args&&... args) {
        auto it = resources.find(id);
        if (it != resources.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        auto resource = std::make_shared<T>(std::forward<Args>(args)...);
        resources[id] = resource;
        resources[id]->resourceKey = id;
        return resource;
    }

    void Unload(const std::string& id) {
        resources.erase(id);
    }

    void Clear();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, std::shared_ptr<Resource>> resources;
};

