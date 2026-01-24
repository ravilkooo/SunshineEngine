#include <Component/TriggerComponent.h>
#include <iostream>

TriggerComponent::TriggerComponent()
    : m_luaCallback(sol::nil)
{
}

void TriggerComponent::SetLuaCallback(sol::function callback)
{
    m_luaCallback = std::move(callback);
}

void TriggerComponent::OnEnter(SE::UUID otherUUID)
{
    if (m_insideObjects.find(otherUUID) != m_insideObjects.end())
    {
        return;  // Already inside, don't trigger again
    }

    m_insideObjects.insert(otherUUID);

    if (m_luaCallback.valid())
    {
        try
        {
            m_luaCallback("enter", otherUUID.m_UUID);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[TriggerComponent::OnEnter] Lua error: " << e.what() << std::endl;
        }
    }
}

void TriggerComponent::OnExit(SE::UUID otherUUID)
{
    auto it = m_insideObjects.find(otherUUID);
    if (it == m_insideObjects.end())
    {
        return;  // Not inside, nothing to exit
    }

    m_insideObjects.erase(it);

    if (m_luaCallback.valid())
    {
        try
        {
            m_luaCallback("exit", otherUUID.m_UUID);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[TriggerComponent::OnExit] Lua error: " << e.what() << std::endl;
        }
    }
}

void TriggerComponent::FromJson(const json& j)
{
    if (j.contains("owner_id"))
    {
        OwnerID = SE::UUID(j["owner_id"].get<uint64_t>());
    }
}

json TriggerComponent_Info::ToJson() const
{
    json j = json::object();

    if (m_assignedComponent)
    {
        j["assigned"] = true;
        j["component_data"] = m_assignedComponent->ToJson();
    }
    else
    {
        j["assigned"] = false;
    }

    return j;
}

void TriggerComponent_Info::FromJson(const json& j)
{
    if (!j.contains("assigned"))
    {
        return;
    }

    bool isAssigned = j["assigned"].get<bool>();

    if (isAssigned && j.contains("component_data"))
    {
        if (!m_assignedComponent)
        {
            m_assignedComponent = eastl::make_shared<TriggerComponent>();
        }

        m_assignedComponent->FromJson(j["component_data"]);
    }
}
