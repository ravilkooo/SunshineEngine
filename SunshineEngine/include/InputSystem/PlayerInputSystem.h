#pragma once

#include <unordered_map>
#include <string>

#include <InputSystem/InputAction.h>
#include <Windows/Keys.h>
#include <Windows/InputDevice.h>

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

    private:

        std::vector<ActionBinding> m_actionBindings;
        std::vector<AxisBinding> m_axisBindings;
    };

    class KeyMapping
    {
        friend class PlayerInputSystem;
    public:
        using ActionBindings = std::unordered_map<Keys, std::string>;
        using AxisBindings = std::unordered_map<Keys, AxisMapping>;

        const ActionBindings& GetActionBindings() const { return m_keyToAction; }
        const AxisBindings& GetAxisBindings() const { return m_keyToAxisAction; }

        KeyMapping();

        //
        // Setup
        //

        void BindAction(Keys key, const std::string& action);
        void BindAxisAction(Keys key, const AxisMapping& axisAction);

        void RemoveActionByKey(Keys key);
        void RemoveAxisActionByKey(Keys key);

        bool HasActionBinding(Keys key) const;
        bool HasAxisBinding(Keys key) const;

        void ClearBindings();

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
    // OS Events
    //

    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    //
    // Frame lifecycle
    //

    void BeginFrame();
    void EndFrame();


    //
    // Query
    //

    bool IsPressed(const std::string& action) const;
    bool IsReleased(const std::string& action) const;
    bool IsHeld(const std::string& action) const;

    float GetValue(const std::string& action) const;

    //
    // Mouse
    //

    float GetMouseDeltaX() const;
    float GetMouseDeltaY() const;

    //
    // Axis
    //

    float GetAxis(const std::string& axis) const;

private:
    //
    // Runtime states
    //

    std::unordered_map<std::string, ActionState> m_actions;
    std::unordered_map<std::string, float> m_axes;

    //
    // Mouse
    //

    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;

    //
    // Mouse position
    //

    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;

    // Mouse wheel

    float m_wheelDelta = 0.0f;
};