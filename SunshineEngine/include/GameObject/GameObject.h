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

#include <GameObject/ParentNode.h>
#include <GameObject/ObjectGroupType.h>

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
        if (HasComponent<T>())
        {
            return GetComponent<T>();
        }
        /*
        if ((typeid(T) == typeid(PhysicsComponent)) && (m_parent.attached == true))
        {
            // auto currObj = this;
            // while (currObj)
            // {
			// 	auto nextObj = currObj->m_parent.ptr;
            //     currObj->DetachFromParent();
            //     currObj = nextObj;
            // }
        }
        */
        auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
        impl->components.emplace_back(component);
        return component;
    }
    
    template<DerivedFromComponent T>
    bool HasComponent() const
    {
        for (auto& comp : impl->components) {
            if (typeid(T) == comp->getType()) return true;
        }
        return false;
    }

    template<DerivedFromComponent T>
    eastl::shared_ptr<T> GetComponent()
    {
        for (auto& comp : impl->components) {
            if (typeid(T) == comp->getType()) {
                return eastl::static_pointer_cast<T>(comp);
            }
        }
        // log << "Component not found";
        // printf("Component not found");
        return nullptr;
    }

    template<DerivedFromComponent T>
    void RemoveComponent() {
        //for (auto& comp : impl->components)
        for (auto it = impl->components.begin(); it != impl->components.end(); it++)
        {
            if (typeid(T) == (*it)->getType()) {
                impl->components.erase(it);
                return;
            }
        }
        // log << "Component not found";
        // printf("Component not found");
        //return nullptr;
    }

    virtual void Update(float deltaTime) {};

    ParentNode<GameObject> m_parent;
    eastl::vector<SE::UUID> m_children;

    void SetParent(ParentNode<GameObject> parent);
    void AttachToParent(bool alreadyLocalTransform = false);
    void DetachFromParent();

protected:
    eastl::unique_ptr<GameObjectImpl> impl;
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

    // Only for inner class methods
    template<DerivedFromComponent_Info T, typename... Args>
    eastl::shared_ptr<T> AddComponent(Args&&... args)
    {
        SE::ComponentType type = T::s_componentType;
        if (type == SE::ComponentType::PHYSICS && m_parent.attached == true)
        {
            /*
            auto currObj = this;
            while (currObj)
            {
				auto nextObj = currObj->m_parent.ptr;
                currObj->DetachFromParent();
                currObj = nextObj;
            }
            */
        }
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

    // Add Component with default values
    void AddDefaultComponent(SE::ComponentType compType);

    ParentNode<GameObject_Info> m_parent;
    void SetParent(ParentNode<GameObject_Info> parent);

    void AttachToParent(bool alreadyLocalTransform = false);

    void DetachFromParent();

    // Serialization
    virtual json ToJson() const;

protected:
    eastl::unique_ptr<GameObject_InfoImpl> impl;
};
