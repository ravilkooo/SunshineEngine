#pragma once

#include <EASTL/functional.h>

#include "AI/Behavior/ActionPatternSystem.h"


using EventTransitionCallback = eastl::function<void(const eastl::string& ToState)>;


class ConditionTransition
{
public:
    explicit ConditionTransition(const eastl::string& InToState) : ToState(InToState) { }

    virtual bool ConditionTransitionCheck(MemoryBoard* MBoard, float DeltaTime) { return false; };

    void SetTargetState(const eastl::string& InToState) { ToState = InToState; };
    const eastl::string& GetTargetState() const { return ToState; }

private:
    eastl::string ToState;
};


class EventTransition
{
public:
    EventTransition(const eastl::string& InToState, EventTransitionCallback InCallback) : ToState(InToState), Callback(InCallback) {}

    void Trigger(const Sunshine::UUID& GOID, MemoryBoard* MBoard);
    bool virtual TriggerCheck(const Sunshine::UUID& GOID, MemoryBoard* MBoard) { return false; };

    void SetTargetState(const eastl::string& NewToState) { ToState = NewToState; };
    const eastl::string& GetTargetState() const { return ToState; };

    void SetCallback(EventTransitionCallback InCallback) { Callback = InCallback; };
private:
    eastl::string ToState;

    EventTransitionCallback Callback;
};


class State
{
    friend class FiniteStateMachine;

public:
    State() { }

    ActionPatternSystem* GetActionPatternSystem() { return APS.get(); }

    void AddConditionTransition(const eastl::shared_ptr<ConditionTransition>& Transition);
    void AddEventTransition(const eastl::shared_ptr<EventTransition>& Transition);

    void RemoveConditionTransition(const eastl::string& TargetState);
    void RemoveEventTransition(const eastl::string& TargetState);

    eastl::shared_ptr<ConditionTransition> CheckConditionTransitions(MemoryBoard* MBoard, float DeltaTime);

    bool Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime);

    virtual void OnStateEnter(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual void OnStateUpdate(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime) {}
    virtual void OnStateAbort(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual void OnStateExit(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}

private:
    eastl::unique_ptr<ActionPatternSystem> APS = eastl::make_unique<ActionPatternSystem>();;

    eastl::vector<eastl::shared_ptr<ConditionTransition>> ConditionTransitions;
    eastl::vector<eastl::shared_ptr<EventTransition>> EventTransitions;

    bool IsRunning = false;
};


class FiniteStateMachine
{
public:
    FiniteStateMachine() { }

    void AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState);
    void RemoveState(const eastl::string& Name);

    void SetInitialState(const eastl::string& Name);
    const eastl::string& GetCurrentStateName() const { return CurrentStateName; }

    void Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime);

    void Abort(const eastl::string& ToState);

private:
    void ChangeState(const Sunshine::UUID& GOID, MemoryBoard* MBoard, const eastl::string& NewState);


    eastl::hash_map<eastl::string, eastl::shared_ptr<State>> States;

    eastl::string CurrentStateName;
    eastl::shared_ptr<State> CurrentState = nullptr;

    eastl::string AfterAbortStateName;

    bool IsClosedForAbort = false;
};
