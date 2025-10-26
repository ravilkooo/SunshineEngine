#pragma once

#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/functional.h>


// This class provides a type-safe key-value storage used by AI agents.
// It allows storing arbitrary data (int, float, bool, string, etc.), 
// subscribing to changes, and accessing values from various subsystems.
class MemoryBoard
{
public:
    // Set a value for the given key.
    // If the value changes, all listeners for this key will be notified.
    template<typename T>
    void Set(const eastl::string& Key, const T& Value);

    // Retrieve a value by key.
    // Returns true if successful, false if not found or type mismatch.
    template<typename T>
    bool Get(const eastl::string& Key, T& OutValue) const;


    bool HasKey(const eastl::string& Key) const { return Data.find(Key) != Data.end(); }
    void RemoveKey(const eastl::string& Key) { Data.erase(Key); }
    void Clear() { Data.clear(); }


    // Base interface for type erasure.
    struct HolderStructInterface { virtual ~HolderStructInterface() = default; };

    using MemoryChangedCallback = eastl::function<void(const MemoryBoard::HolderStructInterface* NewValue)>;
    struct ListenerWrapper { uint64_t Id; MemoryChangedCallback Func; };

    // Subscribe to a key's changes. Returns a unique listener ID.
    // If the key doesn't exist yet, returns UINT64_MAX.
    uint64_t AddListener(const eastl::string& Key, MemoryChangedCallback Callback);

    void RemoveListener(const eastl::string& Key, uint64_t ID);
    void ClearListeners(const eastl::string& Key) { Listeners.erase(Key); }


    // Generate a string representation of the current memory state.
    eastl::string GetState() const;

private:
    // Base interface for type erasure.
    template<typename T>
    struct HolderStruct : HolderStructInterface
    {
        HolderStruct(const T& V) : Value(V) {}
        
        T Value;
    };

    eastl::hash_map<eastl::string, eastl::shared_ptr<HolderStructInterface>> Data;

    eastl::hash_map<eastl::string, eastl::vector<ListenerWrapper>> Listeners;
    uint64_t NextListenerId = 0u;
};