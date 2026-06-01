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
        using AxisBindings = std::unordered_map<Keys,
            std::vector<AxisMapping>>;

        std::string m_name = "Default";

        const ActionBindings& GetActionBindings() const { return m_keyToAction; }
        const AxisBindings& GetAxisBindings() const { return m_keyToAxisAction; }

        KeyMapping();

        //
        // Setup
        //

        void BindAction(Keys key, const std::string& action);
        void BindAxisAction(Keys key, const AxisMapping& axisAction);

        void RemoveActionByKey(Keys key);
        void RemoveAxisByKey(Keys key);

        bool HasActionBinding(Keys key) const;
        bool HasAxisBinding(Keys key) const;

        void ClearBindings();

        // Serialization
        bool FromJson(const json& j);

    private:
        //
        // Key -> Action
        //

        ActionBindings m_keyToAction;

        //
        // Key -> Axis
        //

        AxisBindings   m_keyToAxisAction;
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

    void PressAxis(
        const AxisMapping& mapping);

    void ReleaseAxis(
        const AxisMapping& mapping);

    //
    // Runtime states
    //

    std::unordered_map<std::string, ActionState> m_actions;

    std::unordered_map<Keys, KeyState> m_keys;

    std::unordered_map<std::string, AxisState> m_axes;

    MouseState m_mouse;
};