#include "GameObject/GameObject.h"

GameObject::GameObject() : impl(eastl::make_unique<GameObjectImpl>())
{
	m_UUID = SE::UUID();
};

GameObject::GameObject(SE::UUID uuid) : impl(eastl::make_unique<GameObjectImpl>())
{
	m_UUID = uuid;
};

GameObject::~GameObject()
{
	m_name.clear();
	impl->components.clear();
};

GameObject_Info::GameObject_Info() : impl(eastl::make_unique<GameObject_InfoImpl>())
{
	m_UUID = SE::UUID();
};

GameObject_Info::GameObject_Info(SE::UUID uuid) : impl(eastl::make_unique<GameObject_InfoImpl>())
{
	m_UUID = uuid;
};

GameObject_Info::~GameObject_Info()
{
	m_name.clear();
	impl->components.clear();
};
