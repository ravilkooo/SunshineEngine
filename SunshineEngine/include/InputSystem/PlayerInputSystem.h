#pragma once

#include <unordered_map>
#include <string>

#include <InputSystem/InputAction.h>

enum class Keys;

class PlayerInputSystem
{
public:

    //
    // Setup
    //

    void BindAction(Keys key, const std::string& action);

    void ClearBindings();


    //
    // OS Events
    //

    void HandleKeyDown(Keys key);
    void HandleKeyUp(Keys key);


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


private:

    //
    // Key -> Action
    //

    std::unordered_map<Keys, std::string> m_keyToAction;


    //
    // Runtime states
    //

    std::unordered_map<std::string, ActionState> m_actions;
};