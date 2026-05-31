#include <InputSystem/PlayerInputSystem.h>

PlayerInputSystem::KeyMapping_Info::KeyMapping_Info()
{
    m_actionBindings = std::vector<ActionBinding>();
    m_axisBindings = std::vector<AxisBinding>();
}

PlayerInputSystem::KeyMapping::KeyMapping()
{
    m_keyToAction = std::unordered_map<Keys, std::string>();
    m_keyToAxisAction = std::unordered_map<Keys, AxisMapping>();
}

void PlayerInputSystem::KeyMapping::BindAction(Keys key, const std::string& action)
{
    m_keyToAction[key] = action;
}

void PlayerInputSystem::KeyMapping::BindAxisAction(Keys key, const AxisMapping& axisAction)
{
    m_keyToAxisAction[key] = axisAction;
}

void PlayerInputSystem::KeyMapping::ClearBindings()
{
    m_keyToAction.clear();
}

void PlayerInputSystem::KeyMapping::RemoveActionByKey(Keys key)
{
    m_keyToAction.erase(key);
}

void PlayerInputSystem::KeyMapping::RemoveAxisActionByKey(Keys key)
{
    m_keyToAxisAction.erase(key);
}

bool PlayerInputSystem::KeyMapping::HasActionBinding(Keys key) const
{
    return m_keyToAction.find(key) != m_keyToAction.end();
}

bool PlayerInputSystem::KeyMapping::HasAxisBinding(Keys key) const
{
    return m_keyToAxisAction.find(key) != m_keyToAxisAction.end();
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
    auto it = m_keyMapping.m_keyToAction.find(key);

    if (it == m_keyMapping.m_keyToAction.end())
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
    auto it = m_keyMapping.m_keyToAction.find(key);

    if (it == m_keyMapping.m_keyToAction.end())
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

float PlayerInputSystem::GetMouseDeltaX() const
{
    return m_mouseDeltaX;
}

float PlayerInputSystem::GetMouseDeltaY() const
{
    return m_mouseDeltaY;
}

float PlayerInputSystem::GetAxis(const std::string& axis) const
{
    auto it = m_axes.find(axis);

    if (it == m_axes.end())
        return 0.0f;

    return it->second;
}

void PlayerInputSystem::HandleMouseMove(
    const InputDevice::MouseMoveEventArgs& args)
{
    m_mouseDeltaX += args.Offset.x;
    m_mouseDeltaY += args.Offset.y;


    m_mouseX = args.Position.x;
    m_mouseY = args.Position.y;

    m_wheelDelta = args.WheelDelta;
}
