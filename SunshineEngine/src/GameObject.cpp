#include "GameObject.h"

GameObject::GameObject() : impl(eastl::make_unique<GameObjectImpl>()) {
	m_UUID = Sunshine::UUID();
};

GameObject::GameObject(Sunshine::UUID uuid) : impl(eastl::make_unique<GameObjectImpl>()) {
	m_UUID = uuid;
};

GameObject_Info::GameObject_Info() : impl(eastl::make_unique<GameObject_InfoImpl>()) {
	m_UUID = Sunshine::UUID();
};

GameObject_Info::GameObject_Info(Sunshine::UUID uuid) : impl(eastl::make_unique<GameObject_InfoImpl>()) {
	m_UUID = uuid;
};