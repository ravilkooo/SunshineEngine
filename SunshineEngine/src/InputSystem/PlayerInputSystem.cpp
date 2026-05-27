#include "InputSystem/PlayerInputSystem.h"

void PlayerInputSystem::BindAction(Keys key, const std::string& action)
{
    m_keyToAction[key] = action;
}

void PlayerInputSystem::ClearBindings()
{
    m_keyToAction.clear();
}

void PlayerInputSystem::BeginFrame()
{
    for (auto& [name, state] : m_actions)
    {
        state.Pressed = false;
        state.Released = false;
    }
}

void PlayerInputSystem::HandleKeyDown(Keys key)
{
    auto it = m_keyToAction.find(key);

    if (it == m_keyToAction.end())
        return;

    auto& state = m_actions[it->second];

    if (!state.Held)
    {
        state.Pressed = true;
    }

    state.Held = true;
    state.Value = 1.0f;
}

void PlayerInputSystem::HandleKeyUp(Keys key)
{
    auto it = m_keyToAction.find(key);

    if (it == m_keyToAction.end())
        return;

    auto& state = m_actions[it->second];

    state.Released = true;
    state.Held = false;
    state.Value = 0.0f;
}

bool PlayerInputSystem::IsPressed(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Pressed;
}

bool PlayerInputSystem::IsHeld(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Held;
}

bool PlayerInputSystem::IsReleased(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Released;
}

float PlayerInputSystem::GetValue(const std::string& action) const
{
    auto it = m_actions.find(action);

    if (it == m_actions.end())
        return 0.0f;

    return it->second.Value;
}
