#pragma once

#include "SunshineEngineAPI.h"

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/string.h>
#include <EASTL/map.h>

#include <Graphics/GraphicsResources/Mesh.h>

#include <Component/Component.h>
#include <Component/ComponentType.h>
#include <Component/RenderComponent.h>
#include <Component/TransformComponent.h>
#include <Component/PhysicsComponent.h>
#include <Component/MeshComponent.h>

//#include <Graphics/Renderer/DeferredRenderer.h>

#include <Utils/StringUtils.h>
#include <Utils/UUID.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

template<class T>
concept DerivedFromComponent = std::is_base_of_v<Component, T>;

template<class T>
concept DerivedFromComponent_Info = std::is_base_of_v<Component_Info, T>;

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

    template<DerivedFromComponent T, typename... Args>
    eastl::shared_ptr<T> AddComponent(Args&&... args)
    {
        auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
        impl->components.emplace_back(component);
        return component;
    }
    
    template<DerivedFromComponent T>
    bool HasComponent() const {
        for (auto& comp : impl->components) {
            if (typeid(T) == comp->getType()) return true;
        }
        return false;
    }

    template<DerivedFromComponent T>
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

    // Add Component with default values
    void AddDefaultComponent(SE::ComponentType compType)
    {
        if (impl->components.contains(compType))
        {
            return;
        }
        else
        {
            switch (compType)
            {
            case SE::ComponentType::TRANSFORM:
            case SE::ComponentType::RENDER:
                break;

            case SE::ComponentType::LUA:

                // To-do:
                // Add  #include <Component/LuaComponent.h>
                // Add LuaComponent with default values

                break;

            case SE::ComponentType::PHYSICS:

                // Add PhysicsComponent with default values
            {
                auto tc_info = GetComponent<TransformComponent_Info>();
                auto rc_info = GetComponent<RenderComponent_Info>();

                auto pc_info = AddComponent<PhysicsComponent_Info>(rc_info.get(), tc_info.get());

            }
                break;

            case SE::ComponentType::PERCEPTION:

                // To-do:
                // Add  #include <Component/PerceptionComponent.h>
                // Add PerceptionComponent with default values

                break;

            case SE::ComponentType::BEHAVIOR:

                // To-do:
                // Add  #include <Component/BehaviourController.h>
                // Add BehaviourController with default values

                break;
                
            case SE::ComponentType::MESH:

                // To-do:
                // Add  #include <Component/MeshComponent.h>
                // Add MeshComponent with default values

            {
                auto tc_info = GetComponent<TransformComponent_Info>();
                auto rc_info = GetComponent<RenderComponent_Info>();
                
                auto meshPtr = SE_G::Mesh::CreateUnwrappedBoxMesh_repeat(
                    rc_info->GetDevice(),
                    DXSM::Vector3(1.0f, 1.0f, 1.0f)
                );
                auto meshComp = AddComponent<MeshComponent_Info>(rc_info.get(), tc_info.get(), m_UUID, meshPtr);

            }

                break;

            default:
                break;
            }
        }
    }

    // Only for inner class methods
    template<DerivedFromComponent_Info T, typename... Args>
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

    template<DerivedFromComponent_Info T>
    bool HasComponent() const {
        SE::ComponentType type = T::s_componentType;
        return impl->components.find(type) != impl->components.end();
    }

    template<DerivedFromComponent_Info T>
    eastl::shared_ptr<T> GetComponent() {
        SE::ComponentType type = T::s_componentType;
        auto it = impl->components.find(type);
        if (it != impl->components.end())
            return eastl::static_pointer_cast<T>(it->second);
        // log << "Object hasn't this Component\n";
        return nullptr;
    }

    template<DerivedFromComponent_Info T>
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
