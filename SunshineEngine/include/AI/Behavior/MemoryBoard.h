#pragma once

#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/functional.h>

#include <Utils/UUID.h>
#include <SimpleMath.h>

#ifdef _DEBUG
#include <iostream>
#endif


namespace DXSM = DirectX::SimpleMath;


// This class provides a type-safe key-value storage used by AI agents.
// It allows storing arbitrary data (int, float, bool, string, etc.), 
// subscribing to changes, and accessing values from various subsystems.
class MemoryBoard
{
public:
    // Base interface for type erasure.
    struct HolderStructInterface { virtual ~HolderStructInterface() = default; };

    // Base interface for type erasure.
    template<typename T>
    struct HolderStruct : HolderStructInterface
    {
        HolderStruct(const T& InValue) : Value(InValue) {}

        T Value;
    };

    // --- SETTERS ---
    bool SetInt(const eastl::string& Key, int Value) { return SetTypedValue<int>(Key, Value); }
    bool SetFloat(const eastl::string& Key, float Value) { return SetTypedValue<float>(Key, Value); }
    bool SetBool(const eastl::string& Key, bool Value) { return SetTypedValue<bool>(Key, Value); }
    bool SetString(const eastl::string& Key, const eastl::string& Value) { return SetTypedValue<eastl::string>(Key, Value); }
    bool SetVector3(const eastl::string& Key, const DXSM::Vector3& Value) { return SetTypedValue<DXSM::Vector3>(Key, Value); }
    bool SetUUID(const eastl::string& Key, const Sunshine::UUID& Value) { return SetTypedValue<Sunshine::UUID>(Key, Value); }

    // --- GETTERS ---
    bool GetInt(const eastl::string& Key, int& OutValue) const { return GetTypedValue<int>(Key, OutValue); }
    bool GetFloat(const eastl::string& Key, float& OutValue) const { return GetTypedValue<float>(Key, OutValue); }
    bool GetBool(const eastl::string& Key, bool& OutValue) const { return GetTypedValue<bool>(Key, OutValue); }
    bool GetString(const eastl::string& Key, eastl::string& OutValue) const { return GetTypedValue<eastl::string>(Key, OutValue); }
    bool GetVector3(const eastl::string& Key, DXSM::Vector3& OutValue) const { return GetTypedValue<DXSM::Vector3>(Key, OutValue); }
    bool GetUUID(const eastl::string& Key, Sunshine::UUID& OutValue) const { return GetTypedValue<Sunshine::UUID>(Key, OutValue); }

    // --- GENERAL OPERATIONS ---
    bool HasKey(const eastl::string& Key) const { return Data.find(Key) != Data.end(); }
    void RemoveKey(const eastl::string& Key) { Data.erase(Key); }
    void Clear() { Data.clear(); }


    // --- CALLBACK MANAGEMENT ---
    using MemoryChangedCallback = eastl::function<void(const MemoryBoard::HolderStructInterface* NewValue)>;
    struct CallbackWrapper { uint64_t CallbackId; MemoryChangedCallback Callback; };

    // Subscribe to a key's changes. Returns a unique callback ID.
    // If the key doesn't exist yet, returns UINT64_MAX.
    uint64_t AddCallback(const eastl::string& Key, MemoryChangedCallback Callback);

    void RemoveCallback(const eastl::string& Key, uint64_t CallbackId);
    void ClearCallbacks(const eastl::string& Key) { Callbacks.erase(Key); }

private:
    // --- INTERNAL GENERIC IMPLEMENTATION ---
    template<typename T>
    bool SetTypedValue(const eastl::string& Key, const T& Value)
    {
        bool HasChanged = true;
        bool IsNew = true;

        auto it = Data.find(Key);

        if (it != Data.end())
        {
            IsNew = false;
            auto Holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

            if (!Holder)
            {
#ifdef _DEBUG
                std::cerr << "[Warning] Type mismatch in MemoryBoard::Set key: " << Key.c_str() << "\n";
#endif
                return false;
            }

            if (Holder->Value == Value)
                HasChanged = false;
        }

        if (HasChanged || IsNew)
        {
            Data[Key] = eastl::make_shared<HolderStruct<T>>(Value);

            if (HasChanged && !IsNew)
            {
                auto cbIt = Callbacks.find(Key);

                if (cbIt != Callbacks.end())
                {
                    for (auto& CW : cbIt->second)
                    {
                        CW.Callback(Data[Key].get());
                    }
                }
            }
        }

        return true;
    }

    template<typename T>
    bool GetTypedValue(const eastl::string& Key, T& OutValue) const
    {
        auto it = Data.find(Key);

        if (it == Data.end())
            return false;

        auto Holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

        if (!Holder)
        {
#ifdef _DEBUG
            std::cerr << "[Warning] Type mismatch in MemoryBoard::Get key: " << Key.c_str() << "\n";
#endif
            return false;
        }

        OutValue = Holder->Value;

        return true;
    }


    eastl::hash_map<eastl::string, eastl::shared_ptr<HolderStructInterface>> Data;

    eastl::hash_map<eastl::string, eastl::vector<CallbackWrapper>> Callbacks;
    uint64_t NextCallbackId = 0u;
};
