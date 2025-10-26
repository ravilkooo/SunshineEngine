#include "AI/Behavior/MemoryBoard.h"

#include <iostream>


template<typename T>
void MemoryBoard::Set(const eastl::string& Key, const T& Value)
{
    bool HasChanged = true;
    bool IsNew = true;

    auto it = Data.find(Key);

    // Check if key already exists
    if (it != Data.end())
    {
        IsNew = false;

        auto holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

        if (holder && holder->Value == Value)
            HasChanged = false;
    }

    // Store new value
    Data[Key] = eastl::make_shared<HolderStruct<T>>(Value);

    // Notify listeners if the value has changed
    if (HasChanged && !IsNew)
    {
        auto ListenersIt = Listeners.find(Key);

        if (ListenersIt != Listeners.end())
        {
            for (auto& callback : ListenersIt->second)
                callback(Data[Key].get());
        }
    }
}

template<typename T>
bool MemoryBoard::Get(const eastl::string& Key, T& OutValue) const
{
    auto it = Data.find(Key);

    if (it == Data.end())
        return false;

    auto Holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

    if (!Holder)
    {
        std::cerr << "[Warning] Type mismatch in MemoryBoard::Get for key: " << Key.c_str() << "\n";
        return false;
    }

    OutValue = Holder->value;
}

uint64_t MemoryBoard::AddListener(const eastl::string& Key, MemoryChangedCallback Callback)
{
    auto itData = Data.find(Key);

    // If key does not exist yet, cannot attach listener
    if (itData == Data.end())
        return UINT64_MAX;

    ListenerWrapper LW{ NextListenerId++, Callback };
    Listeners[Key].push_back(LW);

    return LW.Id;
}

void MemoryBoard::RemoveListener(const eastl::string& Key, uint64_t Id)
{
    auto it = Listeners.find(Key);

    if (it == Listeners.end())
        return;

    auto& Vec = it->second;

    // Remove all listeners with given ID
    Vec.erase( eastl::remove_if(Vec.begin(), Vec.end(), 
        [Id](const ListenerWrapper& wrapper) { return wrapper.Id == Id; }), Vec.end() );

    if (Vec.empty())
        Listeners.erase(it);
}

eastl::string MemoryBoard::GetState() const
{
    eastl::string Result = "{ ";

    // Iterate over all stored values and convert to string
    for (auto& [Key, Value] : Data)
    {
        Result += "\"" + Key + "\": ";

        if (auto data = eastl::dynamic_shared_pointer_cast<HolderStruct<int>>(Value))
            Result += eastl::to_string(data->Value);
        else if (auto data = eastl::dynamic_shared_pointer_cast<HolderStruct<float>>(Value))
            Result += eastl::to_string(data->Value);
        else if (auto data = eastl::dynamic_shared_pointer_cast<HolderStruct<bool>>(Value))
            Result += data->Value ? "true" : "false";
        else if (auto data = eastl::dynamic_shared_pointer_cast<HolderStruct<eastl::string>>(Value))
            Result += "\"" + data->Value + "\"";
        else
            Result += "\"<unknown>\"";

        Result += ", ";
    }

    // Remove last comma
    if (Result.size() > 2)
        Result.pop_back(), Result.pop_back();

    Result += " }";

    return Result;
}