#pragma once

#include <string>

class ResourceManager;

class Resource
{
    friend class ResourceManager;
private:
    std::string resourceKey;

public:
    virtual ~Resource() = default;

    const std::string& GetResourceKey() { return resourceKey; }

};
