#include "AI/Behavior/MemoryBoard.h"

// Lua
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA
// ------------------------------------------------------------------------------------------------------

sol::object MemoryBoard::Lua_GetInt(const std::string& Key, sol::this_state L) const 
{
    int v;
    if (!GetInt(Key, v)) return sol::nil;
    return sol::make_object(L, v);
}

sol::object MemoryBoard::Lua_GetFloat(const std::string& Key, sol::this_state L) const 
{
    float v;
    if (!GetFloat(Key, v)) return sol::nil;
    return sol::make_object(L, v);
}

sol::object MemoryBoard::Lua_GetBool(const std::string& Key, sol::this_state L) const 
{
    bool v;
    if (!GetBool(Key, v)) return sol::nil;
    return sol::make_object(L, v);
}

sol::object MemoryBoard::Lua_GetString(const std::string& Key, sol::this_state L) const 
{
    eastl::string v;
    if (!GetString(Key, v)) return sol::nil;
    return sol::make_object(L, std::string(v.c_str()));
}

sol::object MemoryBoard::Lua_GetVector3(const std::string& Key, sol::this_state L) const 
{
    DXSM::Vector3 v;
    if (!GetVector3(Key, v)) return sol::nil;
    return sol::make_object(L, v);
}

sol::object MemoryBoard::Lua_GetUUID(const std::string& Key, sol::this_state L) const
{
    SE::UUID v;
    if (!GetUUID(Key, v)) return sol::nil;
    return sol::make_object(L, v);
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

    Vec.erase( eastl::remove_if(Vec.begin(), Vec.end(), 
        [Id](const CallbackWrapper& CW) { return CW.CallbackId == Id; }), Vec.end() );

    if (Vec.empty())
    {
        Callbacks.erase(it);
    }
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA BINDING
// ------------------------------------------------------------------------------------------------------

#define MB_ADD_FIELD(name) #name, &MemoryBoard::name
#define MB_FIELD_PAIRS

#define MB_ADD_METHOD(k, fn) k, fn
#define MB_METHOD_PAIRS MEMORYBOARD_LUA_METHODS_APPLY(MB_ADD_METHOD)

LUA_REGISTER_TYPE(MemoryBoard, "MemoryBoard", MB_FIELD_PAIRS, MB_METHOD_PAIRS)

#undef MB_ADD_METHOD
#undef MB_FIELD_PAIRS
