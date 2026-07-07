#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <InputSystem/InputAction.h>
#include <Windows/Keys.h>
#include <Windows/InputDevice.h>

#include <SimpleMath.h>
namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PlayerInputSystem
{
public:
    struct ActionBinding
    {
        Keys Key = Keys::None;
        std::string Action;
    };

    struct AxisBinding
    {
        Keys Key = Keys::None;
        std::string Name;
        float Scale = 1.0f;
    };

    class KeyMapping_Info
    {
    public:
        std::string m_name = "Default";

        KeyMapping_Info();

        std::vector<ActionBinding>& GetActionBindings()
        {
            return m_actionBindings;
        }

        std::vector<AxisBinding>& GetAxisBindings()
        {
            return m_axisBindings;
        }

        const std::vector<ActionBinding>& GetActionBindings() const
        {
            return m_actionBindings;
        }

        const std::vector<AxisBinding>& GetAxisBindings() const
        {
            return m_axisBindings;
        }
        
        // Serialization
        json ToJson() const;
        bool FromJson(const json& j);

    private:

        std::vector<ActionBinding> m_actionBindings;
        std::vector<AxisBinding> m_axisBindings;
    };

    class KeyMapping
    {
        friend class PlayerInputSystem;
    public:
        using ActionBindings = std::unordered_map<Keys,
            std::vector<std::string>>;

        std::string m_name = "Default";

        const ActionBindings& GetActionBindings() const { return m_keyToAction; }

        KeyMapping();

        //
        // Setup
        //

        void BindAction(ActionBinding binding);
        void BindAxisAction(AxisBinding binding);

        void RemoveActionByKey(Keys key);
        void RemoveAxisByName(std::string name);

        bool HasActionBinding(Keys key) const;
        bool HasAxis(std::string name) const;

        void ClearBindings();

        // Serialization
        bool FromJson(const json& j);

    private:
        //
        // Key -> Action
        //

        ActionBindings m_keyToAction;

        //
        // Axis name -> Key
        //

        std::unordered_map<std::string, std::vector<AxisKey>> m_axesKeys;
    };

public:
    static PlayerInputSystem& GetInstance()
    {
        static PlayerInputSystem instance;
        return instance;
    }

    // Serialization
    bool FromJson(const json& j);

    //
    //  Key -> Action/Axis mapping
    //

    KeyMapping m_keyMapping;

    //
    // Frame lifecycle
    //

    void BeginFrame();
    void EndFrame();

    //
    // OS Events
    //

    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    //
    // Query
    //

    bool IsPressed(const std::string& action) const;
    bool IsReleased(const std::string& action) const;
    bool IsHeld(const std::string& action) const;
    bool IsKeyHeld(Keys key) const;

    InputActionPhase GetPhase(const std::string& action) const;

    //
    // Axis
    //

    float GetAxis(const std::string& axisName) const;
    DXSM::Vector2 GetAxis2D(const std::string& horizontalAxis,
        const std::string& verticalAxis) const;

    //
    // Mouse
    //

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    float GetMouseX() const;
    float GetMouseY() const;

    float GetMouseWheelDelta() const;

private:
    PlayerInputSystem();

    void PressAction(
        const std::string& action);

    void ReleaseAction(
        const std::string& action);

    //
    // Runtime states
    //

    std::unordered_map<std::string, ActionState> m_actions;

    std::unordered_map<Keys, KeyState> m_keys;

    MouseState m_mouse;
};