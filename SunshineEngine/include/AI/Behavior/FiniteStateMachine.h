#pragma once

#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/functional.h>

#include "AI/Behavior/MemoryBoard.h"


using TransitionCallback = eastl::function<void(const eastl::string& ToState)>;


class ConditionTransition
{
public:
    explicit ConditionTransition(const eastl::string& NewToState, TransitionCallback NewCallback) : ToState(NewToState),
        Callback(NewCallback) {
    }

    bool ConditionTransitionCheck(MemoryBoard* MBoard, float DeltaTime);
    virtual bool ConditionCheck(MemoryBoard* MBoard, float DeltaTime) { return true; };

    void SetTargetState(const eastl::string& NewToState) { ToState = NewToState; };
    const eastl::string& GetTargetState() const { return ToState; }

private:
    eastl::string ToState;

    TransitionCallback Callback;
};

class State
{
public:
    void AddTransition() {}

    //virtual void OnEnter();
    //virtual void OnExit();

    //virtual void OnUpdate(float DeltaTime);

private:
    //AllowedPatterns;
};

class EventTransition
{
public:
    EventTransition(const eastl::string& NewToState, TransitionCallback NewCallback) : ToState(NewToState), Callback(NewCallback) {}

    bool Trigger(MemoryBoard* MBoard);
    bool virtual TriggerCheck(MemoryBoard* MBoard) { return true; };

    void SetTargetState(const eastl::string& NewToState) { ToState = NewToState; };
    const eastl::string& GetTargetState() const { return ToState; };

private:
    TransitionCallback Callback;

    eastl::string ToState;
};

class FiniteStateMachine
{
public:
    void SetMemory(MemoryBoard* NewMBoard) { MBoard = NewMBoard; }

    void SetInitialState(const eastl::string& Name);
    eastl::string GetCurrentStateName() const { return CurrentStateName; };

    void AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState) { States[Name] = NewState; }
    void RemoveState(eastl::string Name);

    void Update(float DeltaTime);
    void ChangeState(const eastl::string& newState);

private:
    MemoryBoard* MBoard = nullptr;

    eastl::hash_map<eastl::string, eastl::shared_ptr<State>> States;
    eastl::string CurrentStateName;
};
