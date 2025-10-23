#pragma once

#include <string>
// надо сюда как-то добавить UUID из движка
// но от ресурсов зависит графика, а от графика зависит движок (циклич зависимость...)
// надо как-то решить пороблему
// либо сделать Id немного другого формата
// (с учётом например особенностей ресурса - путь к файлу, например)

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
