#pragma once

#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>
#include <EASTL/utility.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/type_traits.h>
#include <EASTL/vector.h>
#include <EASTL/functional.h>


using MemoryChangedCallback = eastl::function<void(const eastl::string& Key)>;


class MemoryBoard
{
public:
    template<typename T>
    void Set(const eastl::string& Key, const T& Value) { Data[Key] = eastl::make_shared<HolderStruct<T>>(Value); };
    template<typename T>
    bool Get(const eastl::string& Key, T& OutValue) const;

    bool HasKey(const eastl::string& Key) const { return Data.find(Key) != Data.end(); }
    void RemoveKey(const eastl::string& Key) { Data.erase(Key); }

    void Clear() { Data.clear(); }

    void AddListener(const eastl::string& Key, MemoryChangedCallback Callback) { Listeners[Key].push_back(Callback); }
    void RemoveListeners(const eastl::string& Key) { Listeners.erase(Key); }

    eastl::string DumpSnapshot() const;

private:
    struct HolderStructInterface { virtual ~HolderStructInterface() = default; };

    template<typename T>
    struct HolderStruct : HolderStructInterface
    {
        HolderStruct(const T& V) : Value(V) {}
        
        T Value;
    };

    eastl::hash_map<eastl::string, eastl::shared_ptr<HolderStructInterface>> Data;

    eastl::hash_map<eastl::string, eastl::vector<MemoryChangedCallback>> Listeners;
};