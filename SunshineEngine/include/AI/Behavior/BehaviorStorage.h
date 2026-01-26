#pragma once

#include <vector>
#include <algorithm>


class BehaviorController;


class BehaviorStorage
{
    friend class Game;
    friend class BehaviorController;

public:
    static BehaviorStorage& Get()
    {
        static BehaviorStorage instance;
        return instance;
    }

private:

    void AddBehavior(BehaviorController* BC);

    void RemoveBehavior(BehaviorController* BC);

    void Update(float DeltaTime);

    void Clear() { BCs.clear(); }


    std::vector<BehaviorController*> BCs;
};
