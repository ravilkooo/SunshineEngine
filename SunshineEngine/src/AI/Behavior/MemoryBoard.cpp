#include "AI/Behavior/MemoryBoard.h"

#ifdef _DEBUG
#include <iostream>
#endif


uint64_t MemoryBoard::AddCallback(const eastl::string& Key, MemoryChangedCallback Callback)
{
    auto itData = Data.find(Key);

    // If key does not exist yet, cannot attach callback
    if (itData == Data.end())
        return UINT64_MAX;

    CallbackWrapper LW { NextCallbackId++, Callback };
    Callbacks[Key].push_back(LW);

    return LW.CallbackId;
}

void MemoryBoard::RemoveCallback(const eastl::string& Key, uint64_t Id)
{
    auto it = Callbacks.find(Key);

    if (it == Callbacks.end())
        return;

    auto& Vec = it->second;

    // Remove all callbacks with given ID
    Vec.erase( eastl::remove_if(Vec.begin(), Vec.end(), 
        [Id](const CallbackWrapper& CW) { return CW.CallbackId == Id; }), Vec.end() );

    if (Vec.empty())
        Callbacks.erase(it);
}
