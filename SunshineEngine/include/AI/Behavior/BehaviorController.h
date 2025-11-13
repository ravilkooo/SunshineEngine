#pragma once

#include "AI/Behavior/FiniteStateMachine.h"


class BehaviorController
{
public:
    explicit BehaviorController(const Sunshine::UUID& InGOID) : GOID(InGOID) { };

    eastl::shared_ptr<MemoryBoard> GetMemory() { return MBoard; };

    void Update(float DeltaTime);     

    bool AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState) { return  FSM->AddState(Name, NewState); };
    bool RemoveState(const eastl::string& Name) { return FSM->RemoveState(Name); };

    bool SetInitialState(const eastl::string& Name) { return FSM->SetInitialState(Name); };
    const eastl::string& GetInitialStateName() const { return FSM->GetCurrentStateName(); }

    eastl::shared_ptr<State> GetState(const eastl::string& Name) { return FSM->GetState(Name); }


    bool IsEnabled = true;

private:
    Sunshine::UUID GOID;

    eastl::shared_ptr<MemoryBoard> MBoard;

    eastl::unique_ptr<FiniteStateMachine> FSM = eastl::make_unique<FiniteStateMachine>();;
};
