#pragma once

#include <Windows/Keys.h>
#include <EASTL/string.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * KeyFunctionPair - Represents a mapping between a keyboard key and a Lua function name
 * Used in PlayerObject_Info for editor-based input binding configuration
 */
struct KeyFunctionPair
{
    Keys key = Keys::None;
    eastl::string functionName;

    KeyFunctionPair() = default;

    KeyFunctionPair(Keys k, const eastl::string& fn)
        : key(k), functionName(fn)
    {
    }

    // Serialization
    json ToJson() const
    {
        json j;
        j["key"] = static_cast<int>(key);
        j["functionName"] = functionName.c_str();
        return j;
    }

    static KeyFunctionPair FromJson(const json& j)
    {
        KeyFunctionPair pair;
        if (j.contains("key")) {
            pair.key = static_cast<Keys>(j["key"].get<int>());
        }
        if (j.contains("functionName")) {
            pair.functionName = j["functionName"].get<std::string>().c_str();
        }
        return pair;
    }

    // Comparison
    bool operator==(const KeyFunctionPair& other) const
    {
        return key == other.key && functionName == other.functionName;
    }

    // Check if valid
    bool IsValid() const
    {
        return key != Keys::None && !functionName.empty();
    }

    // Convert key to string for display
    static eastl::string KeyToString(Keys k);
};
