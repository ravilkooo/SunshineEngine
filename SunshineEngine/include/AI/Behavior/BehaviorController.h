#pragma once

#include "AI/Behavior/FiniteStateMachine.h"


class BehaviorController
{
public:
    explicit BehaviorController(const Sunshine::UUID& InGOID) : GOID(InGOID) { };
    ~BehaviorController();

    MemoryBoard& GetMemory() { return MBoard; };
    FiniteStateMachine* GetFiniteStateMachine() { return FSM.get(); }

    void Update(float DeltaTime);          

    bool IsEnabled = true;

private:
    Sunshine::UUID GOID;

    MemoryBoard MBoard; 

    eastl::unique_ptr<FiniteStateMachine> FSM = eastl::make_unique<FiniteStateMachine>();;
};