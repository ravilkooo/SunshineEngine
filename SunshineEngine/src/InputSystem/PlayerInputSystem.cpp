#include <InputSystem/PlayerInputSystem.h>
#include <Utils/StringUtils.h>
#include <Utils/DebugUtils.h>

#pragma region KeyMapping_Info

PlayerInputSystem::KeyMapping_Info::KeyMapping_Info()
{
    m_actionBindings = std::vector<ActionBinding>();
    m_axisBindings = std::vector<AxisBinding>();
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
    m_keyToAction = std::unordered_map<Keys, std::vector<std::string>>();
    m_axesKeys = std::unordered_map<std::string, std::vector<AxisKey>>();
}

bool PlayerInputSystem::KeyMapping::FromJson(const json& j)
{
    try
    {
        // Clear existing bindings
        m_keyToAction.clear();
        m_axesKeys.clear();

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

                BindAction(binding);
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

                BindAxisAction(binding);
            }
        }

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void PlayerInputSystem::KeyMapping::BindAction(ActionBinding binding)
{
    if (!m_keyToAction.contains(binding.Key))
    {
        m_keyToAction[binding.Key] = std::vector<std::string>();
    }
    m_keyToAction[binding.Key].push_back(binding.Action);
}

void PlayerInputSystem::KeyMapping::BindAxisAction(AxisBinding binding)
{
    if (!m_axesKeys.contains(binding.Name))
    {
        m_axesKeys[binding.Name] = std::vector<AxisKey>();
    }
    m_axesKeys[binding.Name].push_back(AxisKey{ binding.Key, binding.Scale });
}

void PlayerInputSystem::KeyMapping::ClearBindings()
{
    m_keyToAction.clear();
    //m_keyToAxisAction.clear();
}

void PlayerInputSystem::KeyMapping::RemoveActionByKey(Keys key)
{
    m_keyToAction.erase(key);
}

void PlayerInputSystem::KeyMapping::RemoveAxisByName(std::string name)
{
    m_axesKeys.erase(name);
}

bool PlayerInputSystem::KeyMapping::HasActionBinding(Keys key) const
{
    return m_keyToAction.find(key) != m_keyToAction.end();
}

bool PlayerInputSystem::KeyMapping::HasAxis(std::string name) const
{
    return m_axesKeys.find(name) != m_axesKeys.end();
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
    PrepareStates();
    return true;
}

void PlayerInputSystem::BeginFrame()
{

}

void PlayerInputSystem::EndFrame() {
    PrepareStates();
}

void PlayerInputSystem::PrepareStates() {
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

#pragma region Handling keys

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

    /*
    for (auto& [name, state] : m_actions)
    {
        printf("isPressed %s: P=%d, H=%d\n", name.c_str(), state.Pressed, state.Held);
    }
    */
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

#pragma endregion

#pragma region Getters

bool PlayerInputSystem::IsPressed(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Pressed;
}

bool PlayerInputSystem::IsReleased(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Released;
}

bool PlayerInputSystem::IsHeld(const std::string& action) const
{
    auto it = m_actions.find(action);

    return it != m_actions.end() && it->second.Held;
}

bool PlayerInputSystem::IsKeyHeld(Keys key) const
{
    auto it = m_keys.find(key);

    return it != m_keys.end() && it->second.Held;
}

InputActionPhase PlayerInputSystem::GetPhase(const std::string& action) const
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

float PlayerInputSystem::GetAxis(const std::string& axisName) const
{
    float value = 0.0f;

    auto it = m_keyMapping.m_axesKeys.find(axisName);
    if (it == m_keyMapping.m_axesKeys.end())
        return 0.0f;

    for (auto& axisKey : it->second)
    {
        if (IsKeyHeld(axisKey.Key))
            value += axisKey.Scale;
    }

    return std::clamp(value, -1.0f, 1.0f);
}

DXSM::Vector2 PlayerInputSystem::GetAxis2D(
    const std::string& horizontal,
    const std::string& vertical) const
{
    return
    DXSM::Vector2{
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

#pragma endregion

void PlayerInputSystem::PressAction(
    const std::string& action)
{
    auto& state = m_actions[action];

    state.PressCount++;

    if (state.PressCount == 1)
    {
        state.Pressed = true;
        state.Held = true;
    }
    else
    {
        state.Pressed = false;
        state.Held = true;
    }
    
    //printf("Press action '%s' : [%d]%d:%d:%d\n", action.c_str(), state.PressCount, state.Held, state.Pressed, state.Released);
}

void PlayerInputSystem::ReleaseAction(
    const std::string& action)
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

#pragma endregion PlayerInputSystem