#include <Windows/InputManager.h>

InputManager::InputManager()
    : m_enabled(true)
{
}

InputManager::~InputManager()
{
}

void InputManager::ProcessKeyDown(Keys key)
{
    if (!m_enabled) return;

    // Add to current keys (if already present, this is a key repeat - we still track it as down)
    m_currentKeys.insert(key);
}

void InputManager::ProcessKeyUp(Keys key)
{
    if (!m_enabled) return;

    // Remove from current keys
    m_currentKeys.erase(key);
}

void InputManager::Update()
{
    if (!m_enabled) {
        Reset();
        return;
    }

    // Compute edge events based on current and previous frame states
    ComputeEdgeEvents();

    // Copy current state to previous for next frame
    m_previousKeys = m_currentKeys;
}

bool InputManager::IsKeyDown(Keys key) const
{
    if (!m_enabled) return false;
    return m_currentKeys.find(key) != m_currentKeys.end();
}

bool InputManager::IsKeyPressed(Keys key) const
{
    if (!m_enabled) return false;
    return m_pressedThisFrame.find(key) != m_pressedThisFrame.end();
}

bool InputManager::IsKeyReleased(Keys key) const
{
    if (!m_enabled) return false;
    return m_releasedThisFrame.find(key) != m_releasedThisFrame.end();
}

void InputManager::Reset()
{
    m_currentKeys.clear();
    m_previousKeys.clear();
    m_pressedThisFrame.clear();
    m_releasedThisFrame.clear();
}

void InputManager::ComputeEdgeEvents()
{
    m_pressedThisFrame.clear();
    m_releasedThisFrame.clear();

    // Find keys that are pressed (in current but not in previous)
    for (const auto& key : m_currentKeys)
    {
        if (m_previousKeys.find(key) == m_previousKeys.end())
        {
            m_pressedThisFrame.insert(key);
        }
    }

    // Find keys that are released (in previous but not in current)
    for (const auto& key : m_previousKeys)
    {
        if (m_currentKeys.find(key) == m_currentKeys.end())
        {
            m_releasedThisFrame.insert(key);
        }
    }
}
