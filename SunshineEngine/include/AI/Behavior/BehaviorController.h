#pragma once

#include <EASTL/string.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/optional.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h>

#include "MemoryBoard.h"
#include <Utils/UUID.h>


enum class EActionCondition
{
    Created,
    Running,
    Succeeded,
    Failed,
    Aborted
};

enum class EActionResult
{
    Running,
    Succeeded,
    Failed
};

enum class EStateResult
{
    Running,
    Finished,
    Aborted
};


using OnDefaultFunc = eastl::function<void(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;
using OnUpdateFunc = eastl::function<void(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)>;
using OnActionUpdateFunc = eastl::function<EActionResult(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)>;
using OnCompleteFunc = eastl::function<void(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, EActionResult Result)>;
using EvaluateUtilityFunc = eastl::function<float(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;

using CheckFunc = eastl::function<bool(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;
using AbortFunc = eastl::function<void(const eastl::string& ToState)>;


class Action
{
    friend class Pattern;

public:
    explicit Action(const eastl::string& InName) : Name(InName) {}


    OnDefaultFunc OnActionStart = nullptr;
    OnActionUpdateFunc OnActionUpdate = nullptr;
    OnDefaultFunc OnActionAbort = nullptr;
    OnCompleteFunc OnActionComplete = nullptr;

    eastl::string Name;

private:
    EActionCondition Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void Abort() { IsAborted = true; }


    bool IsAborted = false;
};


class Pattern
{
    friend class BehaviorController;
    friend class State;

public:
    Pattern() = default;

    void AddAction(eastl::shared_ptr<Action> NewAction);
    void InsertAction(eastl::shared_ptr<Action> NewAction, size_t Index);

    eastl::shared_ptr<Action> GetActionByName(const eastl::string& Name) const;
    eastl::shared_ptr<Action> GetActionByIndex(size_t Index) const;

    bool RemoveActionByName(const eastl::string& Name);
    bool RemoveActionByIndex(size_t Index);

    eastl::vector<eastl::shared_ptr<Action>> GetAllActions() const { return Actions; }


    EvaluateUtilityFunc EvaluateUtility = nullptr;

    OnDefaultFunc OnPatternStart = nullptr;
    OnUpdateFunc OnPatternUpdate = nullptr;
    OnDefaultFunc OnPatternAbort = nullptr;
    OnCompleteFunc OnPatternComplete = nullptr;

private:
    EActionCondition Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void AbortCurrentAction();

    void Reset();


    eastl::vector<eastl::shared_ptr<Action>> Actions;

    size_t CurrentActionIndex = 0;

    bool bStarted = false;
};


class ConditionTransition
{
public:
    explicit ConditionTransition(const eastl::string& InToState, CheckFunc InCheck) : ToState(InToState), Check(InCheck) { }


    CheckFunc Check = nullptr;

    eastl::string ToState;
};


class EventTransition
{
public:
    explicit EventTransition(const eastl::string& InToState, CheckFunc InCheck, BehaviorController* FSM);

    void Trigger(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);

    void ChangeToState(const eastl::string& InToState, BehaviorController* FSM);


    CheckFunc Check = nullptr;

    AbortFunc Abort = nullptr;

    eastl::string ToState;
};


class State
{
    friend class BehaviorController;

public:
    State() = default;

    // Patterns
    bool AddPattern(const eastl::string& Name, eastl::shared_ptr<Pattern> Pattern);
    eastl::shared_ptr<Pattern> GetPattern(const eastl::string& Name);
    bool RemovePattern(const eastl::string& Name);

    eastl::hash_map<eastl::string, eastl::shared_ptr<Pattern>> GetAllPatternsCopy() { return Patterns; };
    //


    OnDefaultFunc OnStateEnter = nullptr;
    OnUpdateFunc OnStateUpdate = nullptr;
    OnDefaultFunc OnStateAbort = nullptr;
    OnDefaultFunc OnStateExit = nullptr;

private:
    // Conditions
    void AddConditionTransition(const eastl::string& InToState, CheckFunc InCheck);
    void AddEventTransition(const eastl::string& InToState, BehaviorController* FSM);

    bool RemoveConditionTransition(const eastl::string& ToState);
    bool RemoveEventTransition(const eastl::string& ToState);
    //

    bool Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    eastl::shared_ptr<ConditionTransition> CheckConditionTransitions(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);


    eastl::vector<eastl::shared_ptr<ConditionTransition>> ConditionTransitions;
    eastl::vector<eastl::shared_ptr<EventTransition>> EventTransitions;

    eastl::hash_map<eastl::string, eastl::shared_ptr<Pattern>> Patterns;
    eastl::shared_ptr<Pattern> CurrentPattern = nullptr;

    bool IsRunning = false;
};


class BehaviorController
{
    friend class EventTransition;

public:
    BehaviorController(const Sunshine::UUID& InGOID) : GOID(InGOID) {};

    // MemoryBoard
    void SetMemoryBoard(const eastl::shared_ptr<MemoryBoard>& InMemoryBoard);
    eastl::shared_ptr<MemoryBoard> GetMemoryBoard() { return MBoard; };
    //

    // States
    bool AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState);
    eastl::shared_ptr<State> GetState(const eastl::string& Name);
    bool RemoveState(const eastl::string& Name);

    bool SetInitialState(const eastl::string& Name);
    const eastl::string& GetCurrentStateName() const { return CurrentStateName; }

    eastl::hash_map<eastl::string, eastl::shared_ptr<State>> GetAllStatesCopy() const { return States; }
    //

    // Conditions
    bool AddConditionTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck);
    bool AddEventTransition(const eastl::string& FromState, const eastl::string& ToState);

    bool ChangeToStateInConditionTransition(const eastl::string& FromState, const eastl::string& OldToState, const eastl::string& NewToState);
    bool ChangeToStateInEventTransition(const eastl::string& FromState, const eastl::string& OldToState, const eastl::string& NewToState);

    bool ChangeCheckFuncInConditionTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck);
    bool ChangeCheckFuncInEventTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck);

    bool RemoveConditionTransition(const eastl::string& FromState, const eastl::string& ToState);
    bool RemoveEventTransition(const eastl::string& FromState, const eastl::string& ToState);
    //


    bool IsEnabled = true;

private:
    void Update(float DeltaTime);

    void Abort(const eastl::string& ToState);

    void ChangeState(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const eastl::string& NewState);


    Sunshine::UUID GOID;
    eastl::shared_ptr<MemoryBoard> MBoard = nullptr;

    eastl::hash_map<eastl::string, eastl::shared_ptr<State>> States;
    eastl::string CurrentStateName;
    eastl::shared_ptr<State> CurrentState = nullptr;

    eastl::string AfterAbortStateName;
    bool IsClosedForAbort = false;
};
