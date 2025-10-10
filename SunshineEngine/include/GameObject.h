#pragma once

#include "SunshineEngineAPI.h"

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/unique_ptr.h>

#include "Component/Component.h"


class GameObjectImpl;

class SUNSHINE_ENGINE_API GameObject
{
public:

    // ================
    // To make SUNSHINE_ENGINE_API work

    GameObject();
    virtual ~GameObject();
    /*
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    */
    // ================

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);

    template<typename T>
    bool HasComponent() const;

    template<typename T>
    T& GetComponent();

    virtual void Tick(float deltaTime) = 0;

protected:
    eastl::unique_ptr<GameObjectImpl> impl;
};

class GameObjectImpl {
public:
    eastl::vector<eastl::unique_ptr<Component>> components;
};

template<typename T, typename... Args>
T& GameObject::AddComponent(Args&&... args) {
    auto component = eastl::make_unique<T>(eastl::forward<Args>(args)...);
    T& ref = *component;
    impl->components.emplace_back(eastl::move(component));
    return ref;
}

template<typename T>
bool GameObject::HasComponent() const {
    for (auto& comp : impl->components) {
        if (typeid(T) == comp->getType()) return true;
    }
    return false;
}

template<typename T>
T& GameObject::GetComponent() {
    for (auto& comp : impl->components) {
        if (typeid(T) == comp->getType()) {
            return *static_cast<T*>(comp.get());
        }
    }
    // assert(false, "Component not found");
}
