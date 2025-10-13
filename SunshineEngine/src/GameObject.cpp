#include "GameObject.h"

GameObject::GameObject() : impl(eastl::make_unique<GameObjectImpl>()) {};
