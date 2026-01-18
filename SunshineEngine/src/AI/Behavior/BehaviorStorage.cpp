#include "AI/Behavior/BehaviorStorage.h"

#include "AI/Behavior/BehaviorController.h"


void BehaviorStorage::AddBehavior(BehaviorController* BC)
{
    BCs.push_back(BC);
}

void BehaviorStorage::RemoveBehavior(BehaviorController* BC)
{
    auto it = std::find(BCs.begin(), BCs.end(), BC);

    if (it != BCs.end())
    {
        BCs.erase(it);
    }
}

void BehaviorStorage::Update(float DeltaTime)
{
    for (BehaviorController* BC : BCs)
    {
        if (BC)
        {
            BC->Update(DeltaTime);
        }
    }
}
