#pragma once

#include <Windows/Keys.h>
#include <Windows/InputDevice.h>

#include <SimpleMath.h> 

#include <EASTL/unordered_map.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

class PlayerObject;

class PlayerController
{
public:
	PlayerObject* m_player;

    float m_stickYawMoveDir = 0.0f;
    float m_stickPitchMoveDir = 0.0f;
    float m_stickYawPitchSpeed = 1.0f;
    
    float m_moveSpeed = 0.1f;
    DXSM::Vector3 m_moveDir = DXSM::Vector3::Zero;

    eastl::unordered_map<Keys, bool> m_isKeyPressed;

    // Lua integration mode
    bool m_useLuaCallbacks = false;

    void SetPlayerObject(PlayerObject* player);

    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);

    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    void UpdatePlayer(float deltaTime);

    // Enable/disable Lua callback mode
    void SetLuaCallbackMode(bool enabled) { m_useLuaCallbacks = enabled; }
    bool IsLuaCallbackMode() const { return m_useLuaCallbacks; }
};