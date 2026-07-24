#include "AI/Behavior/MemoryBoard.h"

// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Serialization
// ------------------------------------------------------------------------------------------------------

json MemoryBoard::ToJson() const
{
    json j = json::object();

    for (const auto& Pair : Data)
    {
        const std::string& Key = Pair.first;
        const auto& Holder = Pair.second;
        json Item = json::object();

        switch (Holder->GetValueType())
        {
        case ValueType::Int:
        {
            auto IntHolder = std::static_pointer_cast<HolderStruct<int>>(Holder);
            Item["type"] = "Int";
            Item["value"] = IntHolder->Value;
            break;
        }
        case ValueType::Float:
        {
            auto FloatHolder = std::static_pointer_cast<HolderStruct<float>>(Holder);
            Item["type"] = "Float";
            Item["value"] = FloatHolder->Value;
            break;
        }
        case ValueType::Bool:
        {
            auto BoolHolder = std::static_pointer_cast<HolderStruct<bool>>(Holder);
            Item["type"] = "Bool";
            Item["value"] = BoolHolder->Value;
            break;
        }
        case ValueType::String:
        {
            auto StringHolder = std::static_pointer_cast<HolderStruct<std::string>>(Holder);
            Item["type"] = "String";
            Item["value"] = StringHolder->Value;
            break;
        }
        case ValueType::Vector3:
        {
            auto VectorHolder = std::static_pointer_cast<HolderStruct<DXSM::Vector3>>(Holder);
            Item["type"] = "Vector3";
            auto val = VectorHolder->Value;
            Item["value"] = { val.x, val.y, val.z };
            break;
        }
        case ValueType::UUID:
        {
            auto UUIDHolder = std::static_pointer_cast<HolderStruct<SE::UUID>>(Holder);
            Item["type"] = "UUID";
            Item["value"] = (uint64_t)UUIDHolder->Value;
            break;
        }
        default:
            continue;
        }

        j[Key] = std::move(Item);
    }

    return j;
}
void MemoryBoard::FromJson(const json& j)
{
    Data.clear();

    if (!j.is_object())
    {
        return;
    }

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        const std::string Key = it.key();
        const json& Item = it.value();

        if (!Item.is_object())
        {
            continue;
        }

        const std::string Type = Item.value("type", "");
        const json& Value = Item["value"];

        if (Type == "Int" && Value.is_number_integer())
        {
            SetInt(Key, Value.get<int>());
        }
        else if (Type == "Float" && Value.is_number())
        {
            SetFloat(Key, Value.get<float>());
        }
        else if (Type == "Bool" && Value.is_boolean())
        {
            SetBool(Key, Value.get<bool>());
        }
        else if (Type == "String" && Value.is_string())
        {
            SetString(Key, Value.get<std::string>());
        }
        else if (Type == "Vector3" && Value.is_array() && Value.size() >= 3)  // j["m_position"].is_array() && j["m_position"].size() >= 3
        {
            DXSM::Vector3 val;
            val.x = Value[0].get<float>();
            val.y = Value[1].get<float>();
            val.z = Value[2].get<float>();

            SetVector3(Key, val);
        }
        else if (Type == "UUID" && (Value.is_number_unsigned() || Value.is_number_integer()))
        {
            SetUUID(Key, SE::UUID(Value.get<uint64_t>()));
        }
    }
}

// ------------------------------------------------------------------------------------------------------
// ---------------------------------- CALLBACK MANAGEMENT
// ------------------------------------------------------------------------------------------------------

uint64_t MemoryBoard::AddCallback(const std::string& Key, const sol::function& Callback)
{
    auto itData = Data.find(Key);

    if (itData == Data.end())
    {
        return UINT64_MAX;
    }

    CallbackWrapper LW { NextCallbackId++, Callback };
    Callbacks[Key].push_back(LW);

    return LW.CallbackId;
}

void MemoryBoard::RemoveCallback(const std::string& Key, uint64_t Id)
{
    auto it = Callbacks.find(Key);

    if (it == Callbacks.end())
    {
        return;
    }

    auto& Vec = it->second;

    Vec.erase( std::remove_if(Vec.begin(), Vec.end(), 
        [Id](const CallbackWrapper& CW) { return CW.CallbackId == Id; }), Vec.end() );

    if (Vec.empty())
    {
        Callbacks.erase(it);
    }
}
