#include <InputSystem/PlayerInputSystem.h>
#include <Utils/StringUtils.h>
#include <Utils/DebugUtils.h>

#pragma region KeyMapping_Info

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

#pragma endregion Editor side

#pragma region KeyMapping

PlayerInputSystem::KeyMapping::KeyMapping()
{
    m_keyToAction = eastl::unordered_map<Keys, eastl::vector<eastl::string>>();
    m_keyToAxisAction = eastl::unordered_map<Keys, eastl::vector<AxisMapping>>();
}

bool PlayerInputSystem::KeyMapping::FromJson(const json& j)
{
    try
    {
        // Clear existing bindings
        m_keyToAction.clear();
        m_keyToAxisAction.clear();

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

                if (!m_keyToAction.contains(binding.Key))
                {
                    m_keyToAction[binding.Key] = eastl::vector<eastl::string>();
                }
                m_keyToAction[binding.Key].push_back(StdToEASTLString(binding.Action));
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

                if (!m_keyToAction.contains(binding.Key))
                {
                    m_keyToAxisAction[binding.Key] = eastl::vector<AxisMapping>();
                }
                m_keyToAxisAction[binding.Key].push_back(
                    AxisMapping{ StdToEASTLString(binding.Name), binding.Scale }
                );
            }
        }

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void PlayerInputSystem::KeyMapping::BindAction(Keys key, const eastl::string& action)
{
    if (!m_keyToAction.contains(key))
    {
        m_keyToAction[key] = eastl::vector<eastl::string>();
    }
    m_keyToAction[key].push_back(action);
}

void PlayerInputSystem::KeyMapping::BindAxisAction(Keys key, const AxisMapping& axisAction)
{
    if (!m_keyToAxisAction.contains(key))
    {
        m_keyToAxisAction[key] = eastl::vector<AxisMapping>();
    }
    m_keyToAxisAction[key].push_back(axisAction);
}

void PlayerInputSystem::KeyMapping::ClearBindings()
{
    m_keyToAction.clear();
}

void PlayerInputSystem::KeyMapping::RemoveActionByKey(Keys key)
{
    m_keyToAction.erase(key);
}

void PlayerInputSystem::KeyMapping::RemoveAxisByKey(Keys key)
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

#pragma endregion Runtime side

#pragma region PlayerInputSystem

PlayerInputSystem::PlayerInputSystem()
{
    m_keyMapping = KeyMapping();
}

bool PlayerInputSystem::FromJson(const json& j)
{
    m_keyMapping.FromJson(j);

    for (auto& a : m_keyMapping.m_keyToAction)
    {
        for (size_t i = 0; i < a.second.size(); i++)
        {
            m_actions[a.second[i]] = ActionState{};
        }
    }

    for (auto& axis : m_keyMapping.m_keyToAxisAction)
    {
        for (size_t i = 0; i < axis.second.size(); i++)
        {
            m_actions[axis.second[i].Name] = ActionState{};
        }
    }

    return true;
}

void PlayerInputSystem::BeginFrame()
{
    for (auto& [key, state] : m_keys)
    {
        state.Pressed = false;
        state.Released = false;
    }

    for (auto& [action, state] : m_actions)
    {
        state.Pressed = false;
        state.Released = false;
    }

    m_mouse.DeltaX = 0.0f;
    m_mouse.DeltaY = 0.0f;

    m_mouse.WheelDelta = 0.0f;
}

void PlayerInputSystem::EndFrame() {
    // To-do
}

void PlayerInputSystem::HandleKeyDown(Keys key)
{
    auto& keyState = m_keys[key];

    if (keyState.Held)
        return;

    keyState.Held = true;
    keyState.Pressed = true;

    auto actionIt =
        m_keyMapping.m_keyToAction.find(key);

    if (actionIt != m_keyMapping.m_keyToAction.end())
    {
        for (const auto& action : actionIt->second)
        {
            PressAction(action);
        }
    }

    auto axisIt =
        m_keyMapping.m_keyToAxisAction.find(key);

    if (axisIt != m_keyMapping.m_keyToAxisAction.end())
    {
        for (const auto& axis : axisIt->second)
        {
            PressAxis(axis);
        }
    }
}

void PlayerInputSystem::HandleKeyUp(Keys key)
{
    auto& keyState = m_keys[key];

    keyState.Held = false;
    keyState.Released = true;

    auto actionIt =
        m_keyMapping.m_keyToAction.find(key);

    if (actionIt != m_keyMapping.m_keyToAction.end())
    {
        for (const auto& action : actionIt->second)
        {
            ReleaseAction(action);
        }
    }

    auto axisIt =
        m_keyMapping.m_keyToAxisAction.find(key);

    if (axisIt != m_keyMapping.m_keyToAxisAction.end())
    {
        for (const auto& axis : axisIt->second)
        {
            ReleaseAxis(axis);
        }
    }
}

void PlayerInputSystem::HandleMouseMove(
    const InputDevice::MouseMoveEventArgs& args)
{
    m_mouse.DeltaX += args.Offset.x;
    m_mouse.DeltaY += args.Offset.y;

    m_mouse.PositionX = args.Position.x;
    m_mouse.PositionY = args.Position.y;

    m_mouse.WheelDelta = args.WheelDelta;

    /*
    printf("Mouse move: Pos[%f,%f], D(%f,%f) W:%f\n",
        m_mouse.PositionX, m_mouse.PositionY,
        m_mouse.DeltaX, m_mouse.DeltaY, m_mouse.WheelDelta);
    */
}

bool PlayerInputSystem::IsPressed(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Pressed;
}

bool PlayerInputSystem::IsReleased(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Released;
}

bool PlayerInputSystem::IsHeld(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Held;
}

InputActionPhase PlayerInputSystem::GetPhase(const eastl::string& action) const
{
    auto it = m_actions.find(action);

    if (it == m_actions.end())
        return InputActionPhase::None;

    const auto& state = it->second;

    if (state.Pressed)
        return InputActionPhase::Pressed;

    if (state.Released)
        return InputActionPhase::Released;

    if (state.Held)
        return InputActionPhase::Held;

    return InputActionPhase::None;
}

float PlayerInputSystem::GetAxis(const eastl::string& axisName) const
{
    auto it = m_axes.find(axisName);

    if (it == m_axes.end())
        return 0.0f;

    return it->second.Value;
}

DXSM::Vector2 PlayerInputSystem::GetAxis2D(
    const eastl::string& horizontal,
    const eastl::string& vertical) const
{
    return
    {
        GetAxis(horizontal),
        GetAxis(vertical)
    };
}

float PlayerInputSystem::GetMouseDeltaX() const
{
    return m_mouse.DeltaX;
}

float PlayerInputSystem::GetMouseDeltaY() const
{
    return m_mouse.DeltaY;
}

float PlayerInputSystem::GetMouseX() const
{
    return m_mouse.PositionX;
}

float PlayerInputSystem::GetMouseY() const
{
    return m_mouse.PositionY;
}

float PlayerInputSystem::GetMouseWheelDelta() const
{
    return m_mouse.WheelDelta;
}

void PlayerInputSystem::PressAction(
    const eastl::string& action)
{
    auto& state = m_actions[action];

    state.PressCount++;

    if (state.PressCount == 1)
    {
        state.Pressed = true;
        state.Held = true;
    }
    
    //printf("Press action '%s' : [%d]%d:%d:%d\n", action.c_str(), state.PressCount, state.Held, state.Pressed, state.Released);
}

void PlayerInputSystem::ReleaseAction(
    const eastl::string& action)
{
    auto& state = m_actions[action];

    if (state.PressCount > 0)
    {
        state.PressCount--;
    }

    if (state.PressCount == 0)
    {
        state.Released = true;
        state.Held = false;
    }

    //printf("Release action '%s' : [%d]%d:%d:%d\n", action.c_str(), state.PressCount, state.Held, state.Pressed, state.Released);
}

void PlayerInputSystem::PressAxis(const AxisMapping& mapping)
{
    auto& axis =
        m_axes[mapping.Name];

    axis.Value += mapping.Scale;

    axis.Value =
        eastl::clamp(
            axis.Value,
            -1.0f,
            1.0f);

    //printf("Press axis '%s': %f\n", mapping.Name.c_str(), axis.Value);
}

void PlayerInputSystem::ReleaseAxis(const AxisMapping& mapping)
{
    auto& axis =
        m_axes[mapping.Name];

    axis.Value -= mapping.Scale;

    axis.Value =
        eastl::clamp(
            axis.Value,
            -1.0f,
            1.0f);

    //printf("Release axis '%s': %f\n", mapping.Name.c_str(), axis.Value);
}

#pragma endregion PlayerInputSystem