#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

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

        eastl::vector<ActionBinding>& GetActionBindings()
        {
            return m_actionBindings;
        }

        eastl::vector<AxisBinding>& GetAxisBindings()
        {
            return m_axisBindings;
        }

        const eastl::vector<ActionBinding>& GetActionBindings() const
        {
            return m_actionBindings;
        }

        const eastl::vector<AxisBinding>& GetAxisBindings() const
        {
            return m_axisBindings;
        }
        
        // Serialization
        json ToJson() const;
        bool FromJson(const json& j);

    private:

        eastl::vector<ActionBinding> m_actionBindings;
        eastl::vector<AxisBinding> m_axisBindings;
    };

    class KeyMapping
    {
        friend class PlayerInputSystem;
    public:
        using ActionBindings = eastl::unordered_map<Keys,
            eastl::vector<eastl::string>>;
        using AxisBindings = eastl::unordered_map<Keys,
            eastl::vector<AxisMapping>>;

        std::string m_name = "Default";

        const ActionBindings& GetActionBindings() const { return m_keyToAction; }
        const AxisBindings& GetAxisBindings() const { return m_keyToAxisAction; }

        KeyMapping();

        //
        // Setup
        //

        void BindAction(Keys key, const eastl::string& action);
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

    bool IsPressed(const eastl::string& action) const;
    bool IsReleased(const eastl::string& action) const;
    bool IsHeld(const eastl::string& action) const;

    InputActionPhase GetPhase(const eastl::string& action) const;

    //
    // Axis
    //

    float GetAxis(const eastl::string& axisName) const;
    DXSM::Vector2 GetAxis2D(const eastl::string& horizontalAxis,
        const eastl::string& verticalAxis) const;
    // float CalcAxisValue(const eastl::string& axis) const;
    

    //
    // Mouse
    //

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    float GetMouseX() const;
    float GetMouseY() const;

    float GetMouseWheelDelta() const;

private:

    void PressAction(
        const eastl::string& action);

    void ReleaseAction(
        const eastl::string& action);

    void PressAxis(
        const AxisMapping& mapping);

    void ReleaseAxis(
        const AxisMapping& mapping);
    //
    // Runtime states
    //

    eastl::unordered_map<eastl::string, ActionState> m_actions;

    eastl::unordered_map<Keys, KeyState> m_keys;

    eastl::unordered_map<eastl::string, AxisState> m_axes;

    MouseState m_mouse;
    
    /*
    struct RuntimeAxisBinding
    {
        Keys Key;
        float Scale;
    };

    eastl::unordered_map<
        eastl::string,
        eastl::vector<RuntimeAxisBinding>> m_axisBindings;
    */
};