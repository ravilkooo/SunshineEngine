#include <InputSystem/PlayerInputSystem.h>
#include <Utils/StringUtils.h>

PlayerInputSystem::KeyMapping_Info::KeyMapping_Info()
{
    m_actionBindings = eastl::vector<ActionBinding>();
    m_axisBindings = eastl::vector<AxisBinding>();
}

json PlayerInputSystem::KeyMapping_Info::ToJson() const
{
    json j;

    j["name"] = m_name;

    // Serialize action bindings
    json actionBindingsArray = json::array();
    for (const auto& binding : m_actionBindings)
    {
        json actionBindingJson;
        actionBindingJson["key"] = static_cast<int>(binding.Key);
        actionBindingJson["action"] = binding.Action;
        actionBindingsArray.push_back(actionBindingJson);
    }
    j["action_bindings"] = actionBindingsArray;

    // Serialize axis bindings
    json axisBindingsArray = json::array();
    for (const auto& binding : m_axisBindings)
    {
        json axisBindingJson;
        axisBindingJson["key"] = static_cast<int>(binding.Key);
        axisBindingJson["name"] = binding.Name;
        axisBindingJson["scale"] = binding.Scale;
        axisBindingsArray.push_back(axisBindingJson);
    }
    j["axis_bindings"] = axisBindingsArray;

    return j;
}

bool PlayerInputSystem::KeyMapping_Info::FromJson(const json& j)
{
    try
    {
        // Clear existing bindings
        m_actionBindings.clear();
        m_axisBindings.clear();

        if (j.contains("name"))
        {
            m_name = j["name"].get<std::string>();
        }

        // Deserialize action bindings
        if (j.contains("action_bindings") && j["action_bindings"].is_array())
        {
            for (const auto& item : j["action_bindings"])
            {
                ActionBinding binding;
                binding.Key = static_cast<Keys>(item.value("key", 0));
                binding.Action = item.value("action", "");
                m_actionBindings.push_back(binding);
            }
        }

        // Deserialize axis bindings
        if (j.contains("axis_bindings") && j["axis_bindings"].is_array())
        {
            for (const auto& item : j["axis_bindings"])
            {
                AxisBinding binding;
                binding.Key = static_cast<Keys>(item.value("key", 0));
                binding.Name = item.value("name", "");
                binding.Scale = item.value("scale", 1.0f);
                m_axisBindings.push_back(binding);
            }
        }

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

PlayerInputSystem::KeyMapping::KeyMapping()
{
    m_keyToAction = eastl::unordered_map<Keys, eastl::string>();
    m_keyToAxisAction = eastl::unordered_map<Keys, AxisMapping>();
}

void PlayerInputSystem::KeyMapping::BindAction(Keys key, const eastl::string& action)
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

bool PlayerInputSystem::IsPressed(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Pressed;
}

bool PlayerInputSystem::IsHeld(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Held;
}

bool PlayerInputSystem::IsReleased(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Released;
}

float PlayerInputSystem::GetValue(const eastl::string& action) const
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

float PlayerInputSystem::GetAxis(const eastl::string& axis) const
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
