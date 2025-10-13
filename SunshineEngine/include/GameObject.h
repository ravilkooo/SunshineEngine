#pragma once

#include "SunshineEngineAPI.h"

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/weak_ptr.h>

#include "Component/Component.h"


class GameObjectImpl;

class SUNSHINE_ENGINE_API GameObject
{
public:

    // ================
    // To make SUNSHINE_ENGINE_API work

    GameObject();
    virtual ~GameObject() = default;
    /*
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    */
    // ================

    template<typename T, typename... Args>
    eastl::shared_ptr<T> AddComponent(Args&&... args);
    //T& AddComponent(Args&&... args);

    template<typename T>
    bool HasComponent() const;

    template<typename T>
    eastl::shared_ptr<T> GetComponent();
    //T& GetComponent();

    virtual void Update(float deltaTime) {};

protected:
    eastl::unique_ptr<GameObjectImpl> impl;
};

class GameObjectImpl {
public:
    eastl::vector<eastl::shared_ptr<Component>> components;
};

template<typename T, typename... Args>
eastl::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
    auto component = eastl::make_shared<T>(eastl::forward<Args>(args)...);
    impl->components.emplace_back(component);
    return component;
}

template<typename T>
bool GameObject::HasComponent() const {
    for (auto& comp : impl->components) {
        if (typeid(T) == comp->getType()) return true;
    }
    return false;
}

template<typename T>
eastl::shared_ptr<T> GameObject::GetComponent() {
    for (auto& comp : impl->components) {
        if (typeid(T) == comp->getType()) {
            return eastl::static_pointer_cast<T>(comp);
        }
    }
    // assert(false, "Component not found");
}
