#pragma once

#include "SunshineEngineAPI.h"

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/string.h>
#include <EASTL/map.h>

#include <Component/Component.h>
#include <Component/ComponentType.h>
#include "Utils/UUID.h"


class GameObjectImpl {
public:
    eastl::vector<eastl::shared_ptr<Component>> components;
};

class SUNSHINE_ENGINE_API GameObject
{
public:

    // ================
    // To make SUNSHINE_ENGINE_API work

    GameObject();
    GameObject(Sunshine::UUID);
    virtual ~GameObject() = default;
    /*
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    */
    // ================

    eastl::string m_name;
    Sunshine::UUID m_UUID;

    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
    eastl::shared_ptr<T> AddComponent(Args&&... args)
    {
        auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
        impl->components.emplace_back(component);
        return component;
    }
    
    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
    bool HasComponent() const {
        for (auto& comp : impl->components) {
            if (typeid(T) == comp->getType()) return true;
        }
        return false;
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component, T>::value>>
    eastl::shared_ptr<T> GetComponent() {
        for (auto& comp : impl->components) {
            if (typeid(T) == comp->getType()) {
                return eastl::static_pointer_cast<T>(comp);
            }
        }
        // assert(false, "Component not found");
    }

    virtual void Update(float deltaTime) {};

protected:
    eastl::unique_ptr<GameObjectImpl> impl;
};

enum class GameObjectGroup {
    Lighting, Shapes, CustomMesh
};

class GameObject_InfoImpl {
public:
    eastl::map<ComponentType, eastl::shared_ptr<Component_Info>> components;
};

class GameObject_Info {
public:
    GameObject_Info();
    GameObject_Info(Sunshine::UUID uuid);
    virtual ~GameObject_Info() = default;

    GameObjectGroup m_group;
    eastl::string m_name;
    Sunshine::UUID m_UUID;

    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    eastl::shared_ptr<T> AddComponent(Args&&... args) {
        ComponentType type = T::StaticComponentType();
        // Check if already has this type
        if (HasComponent<T>()) {
            // log << "Object already has this Component\n";
            return GetComponent<T>();
        }

        auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
        impl->components[type] = component;
        return component;
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    bool HasComponent() const {
        ComponentType type = T::StaticComponentType();
        return impl->components.find(type) != impl->components.end();
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    eastl::shared_ptr<T> GetComponent() {
        ComponentType type = T::StaticComponentType();
        auto it = impl->components.find(type);
        if (it != impl->components.end())
            return eastl::static_pointer_cast<T>(it->second);
        // log << "Object hasn't this Component\n";
        return nullptr;
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    void RemoveComponent() {
        ComponentType type = T::StaticComponentType();
        impl->components.erase(type);
    }

protected:
    eastl::unique_ptr<GameObject_InfoImpl> impl;
};
