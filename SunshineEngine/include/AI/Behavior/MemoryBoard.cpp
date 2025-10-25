#include "MemoryBoard.h"


template<typename T>
void Set(const eastl::string& Key, const T& Value)
{
    bool HasChanged = true;

    auto it = Data.find(Key);

    if (it != Data.end())
    {
        auto holder = eastl::dynamic_shared_pointer_cast<HolderStruct<T>>(it->second);

        if (holder && holder->Value == Value)
            HasChanged = false;
    }

    Data[Key] = eastl::make_shared<HolderStruct<T>>(Value);

    if (HasChanged)
    {
        auto listenersIt = Listeners.find(Key);
        if (listenersIt != Listeners.end())
        {
            for (auto& callback : listenersIt->second)
                callback(Key);
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
        std::cerr << "[Warning]\n";
        return false;
    }

    OutValue = Holder->value;
}

eastl::string MemoryBoard::DumpSnapshot() const
{
    eastl::string Result = "{ ";

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

    if (Result.size() > 2)
        Result.pop_back(), Result.pop_back();

    Result += " }";

    return Result;
}