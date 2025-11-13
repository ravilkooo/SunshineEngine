#pragma once

#include <EASTL/functional.h>

#include "AI/Behavior/ActionPatternSystem.h"


using CheckFunc = eastl::function<bool(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;
using AbortFunc = eastl::function<void(const eastl::string& ToState)>;


class ConditionTransition
{
    friend class State;

public:
    explicit ConditionTransition(const eastl::string& InToState, CheckFunc InCheck) : ToState(InToState), Check(InCheck) { }


    CheckFunc Check = nullptr;

    eastl::string ToState;
};


class EventTransition
{
    //friend class State;

public:
    explicit EventTransition(const eastl::string& InToState, FiniteStateMachine* FSM);

    void Trigger(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);


    CheckFunc Check = nullptr;

    eastl::string ToState;

private:
    AbortFunc Abort = nullptr;
};


class State
{
    friend class FiniteStateMachine;

public:
    State() { }


    OnDefaultFunc OnStateEnter = nullptr;
    OnUpdateFunc OnStateUpdate = nullptr;
    OnDefaultFunc OnStateAbort = nullptr;
    OnDefaultFunc OnStateExit = nullptr;

private:
    eastl::shared_ptr<ConditionTransition> AddConditionTransition(const eastl::string& InToState);
    eastl::shared_ptr<EventTransition> AddEventTransition(const eastl::string& InToState, FiniteStateMachine* FSM);

    void RemoveConditionTransition(const eastl::string& ToState);
    void RemoveEventTransition(const eastl::string& ToState);

    bool Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    ActionPatternSystem* GetActionPatternSystem() { return APS.get(); }

    eastl::shared_ptr<ConditionTransition> CheckConditionTransitions(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);


    eastl::unique_ptr<ActionPatternSystem> APS = eastl::make_unique<ActionPatternSystem>();;

    eastl::vector<eastl::shared_ptr<ConditionTransition>> ConditionTransitions;
    eastl::vector<eastl::shared_ptr<EventTransition>> EventTransitions;

    bool IsRunning = false;
};


class FiniteStateMachine
{
public:
    FiniteStateMachine() = default;

    bool AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState);
    bool RemoveState(const eastl::string& Name);

    bool SetInitialState(const eastl::string& Name);
    const eastl::string& GetCurrentStateName() const { return CurrentStateName; }

    eastl::shared_ptr<ConditionTransition>  AddConditionTransition(const eastl::string& FromState, const eastl::string& ToState);
    eastl::shared_ptr<EventTransition> AddEventTransition(const eastl::string& FromState, const eastl::string& ToState);

    void RemoveConditionTransition(const eastl::string& FromState, const eastl::string& ToState);
    void RemoveEventTransition(const eastl::string& FromState, const eastl::string& ToState);



    void Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void Abort(const eastl::string& ToState);

    void ChangeState(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const eastl::string& NewState);

    eastl::shared_ptr<State> GetState(const eastl::string& Name);


    eastl::hash_map<eastl::string, eastl::shared_ptr<State>> States;

    eastl::string CurrentStateName;
    eastl::shared_ptr<State> CurrentState = nullptr;

    eastl::string AfterAbortStateName;

    bool IsClosedForAbort = false;
};
