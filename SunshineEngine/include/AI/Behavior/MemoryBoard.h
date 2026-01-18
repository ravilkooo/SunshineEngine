#pragma once

// Engine
#include <Utils/UUID.h>
#include <SimpleMath.h>

// C++
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

// Lua
#include <sol/sol.hpp>


namespace DXSM = DirectX::SimpleMath;


// This class provides a type-safe key-value storage used by AI agents.
// It allows storing arbitrary data (int, float, bool, string, etc.), 
// subscribing to changes, and accessing values from various subsystems.
class MemoryBoard
{

public:
    MemoryBoard() {}

    // Base interface for type erasure.
    struct HolderStructInterface 
    { 
        virtual ~HolderStructInterface() = default; 

        virtual const std::type_info& GetType() const = 0;
    };

    // Base interface for type erasure.
    template<typename T>
    struct HolderStruct : HolderStructInterface
    {
        HolderStruct(const T& InValue) : Value(InValue) {}

        const std::type_info& GetType() const override { return typeid(T); }

        T Value;
    };

    // --- SETTERS ---
    bool SetInt(const std::string& Key, int Value)                      { return SetTypedValue<int>(Key, Value); }
    bool SetFloat(const std::string& Key, float Value)                  { return SetTypedValue<float>(Key, Value); }
    bool SetBool(const std::string& Key, bool Value)                    { return SetTypedValue<bool>(Key, Value); }
    bool SetString(const std::string& Key, const std::string& Value)    { return SetTypedValue<std::string>(Key, Value); }
    bool SetVector3(const std::string& Key, const DXSM::Vector3& Value) { return SetTypedValue<DXSM::Vector3>(Key, Value); }
    bool SetUUID(const std::string& Key, const SE::UUID& Value)         { return SetTypedValue<SE::UUID>(Key, Value); }
    //

    // --- GETTERS ---
    bool GetInt(const std::string& Key, int& OutValue) const               { return GetTypedValue<int>(Key, OutValue); }
    bool GetFloat(const std::string& Key, float& OutValue) const           { return GetTypedValue<float>(Key, OutValue); }
    bool GetBool(const std::string& Key, bool& OutValue) const             { return GetTypedValue<bool>(Key, OutValue); }
    bool GetString(const std::string& Key, std::string& OutValue) const    { return GetTypedValue<std::string>(Key, OutValue); }
    bool GetVector3(const std::string& Key, DXSM::Vector3& OutValue) const { return GetTypedValue<DXSM::Vector3>(Key, OutValue); }
    bool GetUUID(const std::string& Key, SE::UUID& OutValue) const         { return GetTypedValue<SE::UUID>(Key, OutValue); }
    //

    // --- LUA-FRIENDLY GETTERS ---
    sol::object Lua_GetInt(const std::string& Key, sol::this_state L) const;
    sol::object Lua_GetFloat(const std::string& Key, sol::this_state L) const;
    sol::object Lua_GetBool(const std::string& Key, sol::this_state L) const;
    sol::object Lua_GetString(const std::string& Key, sol::this_state L) const;
    sol::object Lua_GetVector3(const std::string& Key, sol::this_state L) const;
    sol::object Lua_GetUUID(const std::string& Key, sol::this_state L) const;
    //

    // --- GENERAL OPERATIONS ---
    bool HasKey(const std::string& Key) const { return Data.find(Key) != Data.end(); }
    void RemoveKey(const std::string& Key)    { Data.erase(Key); }
    void Clear()                              { Data.clear(); }
    //

    // --- CALLBACK MANAGEMENT ---
    struct CallbackWrapper { uint64_t CallbackId; sol::function Callback; };

    // Subscribe to a key's changes. Returns a unique callback ID.
    // If the key doesn't exist yet, returns UINT64_MAX.
    uint64_t AddCallback(const std::string& Key, const sol::function& Callback);
    void RemoveCallback(const std::string& Key, uint64_t CallbackId);
    void ClearCallbacks(const std::string& Key) { Callbacks.erase(Key); }
    //

private:
    // --- INTERNAL GENERIC IMPLEMENTATION ---
    template<typename T>
    bool SetTypedValue(const std::string& Key, const T& Value)
    {
        auto it = Data.find(Key);

        if (it == Data.end())
        {
            Data[Key] = std::make_shared<HolderStruct<T>>(Value);
            return true;
        }

        auto Holder = std::dynamic_pointer_cast<HolderStruct<T>>(it->second);

        if (!Holder)
        {
            std::cerr << "[Warning] MemoryBoard::Set: Type mismatch in key: " << Key.c_str() << "\n";
            return false;
        }

        if (Holder->Value == Value)
        {
            return true;
        }

        Holder->Value = Value;

        auto cbIt = Callbacks.find(Key);

        if (cbIt != Callbacks.end())
        {
            for (auto& CW : cbIt->second)
            {
                CW.Callback(Holder->Value);
            }
        }

        return true;
    }

    template<typename T>
    bool GetTypedValue(const std::string& Key, T& OutValue) const
    {
        auto it = Data.find(Key);

        if (it == Data.end())
        {
            std::cerr << "[Warning] MemoryBoard::Get: key does not exist: " << Key.c_str() << "\n";
            return false;
        }

        auto Holder = std::dynamic_pointer_cast<HolderStruct<T>>(it->second);

        if (!Holder)
        {
            std::cerr << "[Warning] MemoryBoard::Get: Type mismatch for key: " << Key.c_str() << "\n";
            return false;
        }

        OutValue = Holder->Value;
        return true;
    }
    //


    std::unordered_map<std::string, std::shared_ptr<HolderStructInterface>> Data;

    std::unordered_map<std::string, std::vector<CallbackWrapper>> Callbacks;
    uint64_t NextCallbackId = 0u;
};


#ifndef MEMORYBOARD_LUA_METHODS_APPLY
#define MEMORYBOARD_LUA_METHODS_APPLY(FM) \
    FM("setInt",         &MemoryBoard::SetInt) , \
    FM("setFloat",       &MemoryBoard::SetFloat) , \
    FM("setBool",        &MemoryBoard::SetBool) , \
    FM("setString",      &MemoryBoard::SetString) , \
    FM("setVector3",     &MemoryBoard::SetVector3) , \
    FM("setUUID",        &MemoryBoard::SetUUID) , \
    FM("getInt",         &MemoryBoard::Lua_GetInt) , \
    FM("getFloat",       &MemoryBoard::Lua_GetFloat) , \
    FM("getBool",        &MemoryBoard::Lua_GetBool) , \
    FM("getString",      &MemoryBoard::Lua_GetString) , \
    FM("getVector3",     &MemoryBoard::Lua_GetVector3) , \
    FM("getUUID",        &MemoryBoard::Lua_GetUUID) , \
    FM("hasKey",         &MemoryBoard::HasKey) , \
    FM("removeKey",      &MemoryBoard::RemoveKey) , \
    FM("clear",          &MemoryBoard::Clear) , \
    FM("addCallback",    &MemoryBoard::AddCallback) , \
    FM("removeCallback", &MemoryBoard::RemoveCallback) , \
    FM("clearCallbacks", &MemoryBoard::ClearCallbacks)
#endif
