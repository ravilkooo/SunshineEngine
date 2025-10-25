#pragma once

#include "MemoryBoard.h"
#include "FiniteStateMachine.h"
#include "ActionPatternSystem.h"


class BehaviorController
{
public:
    BehaviorController() { Initialize(); };
    ~BehaviorController() { Reset(); };

    void Initialize();    
    void Reset();

    void Tick(float DeltaTime);                 

    MemoryBoard& GetMemory() { return MBoard; };
    FiniteStateMachine* GetFiniteStateMachine() { return FSM.get(); }
    ActionPatternSystem* GetActionSystem() { return APS.get(); }

    void SetEnabled(bool NewEnable) { IsEnabled = NewEnable; };
    bool GetIsEnabled() const { return IsEnabled; };

private:
    bool IsEnabled = true;

    MemoryBoard MBoard; 
    eastl::unique_ptr<FiniteStateMachine> FSM;
    eastl::unique_ptr<ActionPatternSystem> APS; 
};