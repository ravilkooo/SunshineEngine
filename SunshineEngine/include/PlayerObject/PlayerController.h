#pragma once

#include <Windows/Keys.h>
#include <Windows/InputDevice.h>
#include <Windows/InputManager.h>

#include <SimpleMath.h> 

#include <EASTL/unordered_map.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;

class PlayerObject;

class PlayerController
{
public:
	PlayerObject* m_player = nullptr;

    float m_stickYawMoveDir = 0.0f;
    float m_stickPitchMoveDir = 0.0f;
    float m_stickYawPitchSpeed = 1.0f;
    
    float m_moveSpeed = 0.1f;
    DXSM::Vector3 m_moveDir = DXSM::Vector3::Zero;

    eastl::unordered_map<Keys, bool> m_isKeyPressed;
    // Robust input system with edge detection
    InputManager m_inputManager;

    // Lua integration mode
    bool m_useLuaCallbacks = false;

    void SetPlayerObject(PlayerObject* player);

    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    void ExecuteAllOnKeyDown();

    void UpdatePlayer(float deltaTime);

    // Enable/disable Lua callback mode
    void SetLuaCallbackMode(bool enabled) { m_useLuaCallbacks = enabled; }
    bool IsLuaCallbackMode() const { return m_useLuaCallbacks; }
};
