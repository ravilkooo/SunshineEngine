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
#include <Utils/UUID.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace SE_G {
    class DeferredRenderer;
    class Camera;
}

class GameObjectImpl {
public:
    // to-do: make vector of unique? or not to do?
    eastl::vector<eastl::shared_ptr<Component>> components;
};

class SUNSHINE_ENGINE_API GameObject
{
public:

    // ================
    // To make SUNSHINE_ENGINE_API work

    GameObject();
    GameObject(SE::UUID);
    virtual ~GameObject();
    /*
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    */
    // ================

    eastl::string m_name;
    SE::UUID m_UUID;

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
        // log << "Component not found";
        printf("Component not found");
        return nullptr;
    }

    virtual void Update(float deltaTime) {};

protected:
    eastl::unique_ptr<GameObjectImpl> impl;
};

enum class GameObjectGroup {
    Lighting, Shapes, CustomMesh, Other
};

NLOHMANN_JSON_SERIALIZE_ENUM(GameObjectGroup, {
    {GameObjectGroup::Lighting, "Lighting"},
    {GameObjectGroup::Shapes, "Shapes"},
    {GameObjectGroup::CustomMesh, "CustomMesh"},
    {GameObjectGroup::Other, "Other"},
    })


enum class LightObjectType {
    PointLight, DirectionalLight, SkyBox, AmbientLight, 
};

NLOHMANN_JSON_SERIALIZE_ENUM(LightObjectType, {
    {LightObjectType::PointLight, "PointLight"},
    {LightObjectType::DirectionalLight, "DirectionalLight"},
    {LightObjectType::SkyBox, "SkyBox"},
    {LightObjectType::AmbientLight, "AmbientLight"},
    })

enum class ShapeObjectType {
    Box, Sphere, Geosphere,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ShapeObjectType, {
    {ShapeObjectType::Box, "Box"},
    {ShapeObjectType::Sphere, "Sphere"},
    {ShapeObjectType::Geosphere, "Geosphere"},
    })

struct ObjectType {
    union {
        LightObjectType m_asLight;
        ShapeObjectType m_asShape;
    };

    ObjectType() {}

    ObjectType(GameObjectGroup objGroup, const json& j)
    {
        switch (objGroup)
        {
        case GameObjectGroup::Lighting:
            m_asLight = j; // j["m_type"];
            break;
        case GameObjectGroup::Shapes:
            m_asShape = j; // j["m_type"];
            break;
        case GameObjectGroup::CustomMesh:
            break;
        case GameObjectGroup::Other:
            break;
        default:
            break;
        }
    }
};

class GameObject_InfoImpl {
public:
    eastl::map<SE::ComponentType, eastl::shared_ptr<Component_Info>> components;
};

class GameObject_Info {
public:
    GameObject_Info();
    GameObject_Info(SE::UUID uuid);
    virtual ~GameObject_Info();

    GameObjectGroup m_group;
    ObjectType m_type;
    eastl::string m_name;
    SE::UUID m_UUID;

    template<typename T, typename... Args,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    eastl::shared_ptr<T> AddComponent(Args&&... args) {
        //SE::ComponentType type = T::StaticComponentType();
        // Check if already has this type
        if (HasComponent<T>()) {
            // log << "Object already has this Component\n";
            return GetComponent<T>();
        }

        auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
        impl->components[component->ComponentType()] = component;
        return component;
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    bool HasComponent() const {
        SE::ComponentType type = T::s_componentType;
        return impl->components.find(type) != impl->components.end();
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    eastl::shared_ptr<T> GetComponent() {
        SE::ComponentType type = T::s_componentType;
        auto it = impl->components.find(type);
        if (it != impl->components.end())
            return eastl::static_pointer_cast<T>(it->second);
        // log << "Object hasn't this Component\n";
        return nullptr;
    }

    template<typename T,
        typename = std::enable_if_t<std::is_base_of<Component_Info, T>::value>>
    void RemoveComponent() {
        SE::ComponentType type = T::s_componentType;
        impl->components.erase(type);
    }

    // Serialization
    virtual json ToJson() const;
    /*
    static eastl::unique_ptr<GameObject_Info> FromJson(
        SE_G::DeferredRenderer* renderSystem,
        eastl::shared_ptr<SE_G::Camera> camera, const json& j);
        */

protected:
    eastl::unique_ptr<GameObject_InfoImpl> impl;
};
