#pragma once

// EASTL
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/functional.h>

// Engine
#include <Utils/UUID.h>
#include <SimpleMath.h>

// C++
#include <iostream>

// Lua
#include <sol/sol.hpp>


namespace DXSM = DirectX::SimpleMath;


// This class provides a type-safe key-value storage used by AI agents.
// It allows storing arbitrary data (int, float, bool, string, etc.), 
// subscribing to changes, and accessing values from various subsystems.
class MemoryBoard
{
public:
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
    bool SetInt(const eastl::string& Key, int Value) { return SetTypedValue<int>(Key, Value); }
    bool SetFloat(const eastl::string& Key, float Value) { return SetTypedValue<float>(Key, Value); }
    bool SetBool(const eastl::string& Key, bool Value) { return SetTypedValue<bool>(Key, Value); }
    bool SetString(const eastl::string& Key, const eastl::string& Value) { return SetTypedValue<eastl::string>(Key, Value); }
    bool SetVector3(const eastl::string& Key, const DXSM::Vector3& Value) { return SetTypedValue<DXSM::Vector3>(Key, Value); }
    bool SetUUID(const eastl::string& Key, const SE::UUID& Value) { return SetTypedValue<SE::UUID>(Key, Value); }
    //

    // --- GETTERS ---
    bool GetInt(const eastl::string& Key, int& OutValue) const { return GetTypedValue<int>(Key, OutValue); }
    bool GetFloat(const eastl::string& Key, float& OutValue) const { return GetTypedValue<float>(Key, OutValue); }
    bool GetBool(const eastl::string& Key, bool& OutValue) const { return GetTypedValue<bool>(Key, OutValue); }
    bool GetString(const eastl::string& Key, eastl::string& OutValue) const { return GetTypedValue<eastl::string>(Key, OutValue); }
    bool GetVector3(const eastl::string& Key, DXSM::Vector3& OutValue) const { return GetTypedValue<DXSM::Vector3>(Key, OutValue); }
    bool GetUUID(const eastl::string& Key, SE::UUID& OutValue) const { return GetTypedValue<SE::UUID>(Key, OutValue); }
    //

    // --- GENERAL OPERATIONS ---
    bool HasKey(const eastl::string& Key) const { return Data.find(Key) != Data.end(); }
    void RemoveKey(const eastl::string& Key) { Data.erase(Key); }
    void Clear() { Data.clear(); }
    //

    // --- CALLBACK MANAGEMENT ---
    struct CallbackWrapper { uint64_t CallbackId; sol::function Callback; };

    // Subscribe to a key's changes. Returns a unique callback ID.
    // If the key doesn't exist yet, returns UINT64_MAX.
    uint64_t AddCallback(const eastl::string& Key, const sol::function& Callback);

    void RemoveCallback(const eastl::string& Key, uint64_t CallbackId);
    void ClearCallbacks(const eastl::string& Key) { Callbacks.erase(Key); }
    //

private:
    // --- INTERNAL GENERIC IMPLEMENTATION ---
    template<typename T>
    bool SetTypedValue(const eastl::string& Key, const T& Value)
    {
        auto it = Data.find(Key);

        if (it == Data.end())
        {
            Data[Key] = eastl::make_shared<HolderStruct<T>>(Value);
            return true;
        }

        auto Holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

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
                CW.Callback(Holder.get());
            }
        }

        return true;
    }

    template<typename T>
    bool GetTypedValue(const eastl::string& Key, T& OutValue) const
    {
        auto it = Data.find(Key);

        if (it == Data.end())
        {
            std::cerr << "[Warning] MemoryBoard::Get: key does not exist: " << Key.c_str() << "\n";
            return false;
        }

        auto Holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

        if (!Holder)
        {
            std::cerr << "[Warning] MemoryBoard::Get: Type mismatch for key: " << Key.c_str() << "\n";
            return false;
        }

        OutValue = Holder->Value;
        return true;
    }
    //


    eastl::hash_map<eastl::string, eastl::shared_ptr<HolderStructInterface>> Data;

    eastl::hash_map<eastl::string, eastl::vector<CallbackWrapper>> Callbacks;
    uint64_t NextCallbackId = 0u;
};
