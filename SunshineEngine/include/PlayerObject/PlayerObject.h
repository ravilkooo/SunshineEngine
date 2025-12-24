#pragma once
#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <GameObject/GameObject.h>
#include <Graphics/Utils/Camera.h>
#include <PlayerObject/PlayerLuaKeyActionsMapping.h>

class PlayerObject : public GameObject
{
public:
	eastl::shared_ptr<SE_G::Camera> m_playerCamera;

	PlayerLuaKeyActionsMapping m_luaActionMapping;

};