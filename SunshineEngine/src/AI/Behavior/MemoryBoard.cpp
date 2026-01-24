#include "AI/Behavior/MemoryBoard.h"


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

    Vec.erase( std::remove_if(Vec.begin(), Vec.end(), 
        [Id](const CallbackWrapper& CW) { return CW.CallbackId == Id; }), Vec.end() );

    if (Vec.empty())
    {
        Callbacks.erase(it);
    }
}
