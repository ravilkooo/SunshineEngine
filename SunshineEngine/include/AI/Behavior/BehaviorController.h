#pragma once

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/optional.h>
#include <EASTL/utility.h>
#include <EASTL/functional.h>

#include "MemoryBoard.h"
#include <Utils/UUID.h>
#include <Component/Component.h>


enum class EActionCondition
{
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


using OnDefaultFunc = eastl::function<void(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;
using OnUpdateFunc = eastl::function<void(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)>;
using OnActionUpdateFunc = eastl::function<EActionResult(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)>;
using OnCompleteFunc = eastl::function<void(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, EActionResult Result)>;
using EvaluateUtilityFunc = eastl::function<float(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;

using CheckFunc = eastl::function<bool(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)>;
using AbortFunc = eastl::function<void(const std::string& ToState)>;


class Action
{
    friend class Pattern;

public:
    explicit Action(const std::string& InName) : Name(InName) {}


    // --- Lua friendly ---
    const std::string& GetName() const { return Name; }

    void Lua_SetOnStart(const sol::function& Func);
    void Lua_SetOnUpdate(const sol::function& Func);
    void Lua_SetOnAbort(const sol::function& Func);
    void Lua_SetOnComplete(const sol::function& Func);
    //

    std::string Name;

    OnDefaultFunc OnActionStart = nullptr;
    OnActionUpdateFunc OnActionUpdate = nullptr;
    OnDefaultFunc OnActionAbort = nullptr;
    OnCompleteFunc OnActionComplete = nullptr;

private:
    EActionCondition Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

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

    eastl::shared_ptr<Action> GetActionByName(const std::string& Name) const;
    eastl::shared_ptr<Action> GetActionByIndex(size_t Index) const;

    bool RemoveActionByName(const std::string& Name);
    bool RemoveActionByIndex(size_t Index);

    eastl::vector<eastl::shared_ptr<Action>> GetAllActions() const { return Actions; }

    // --- Lua friendly ---
    void Lua_SetEvaluateUtility(const sol::function& Func);

    void Lua_SetOnStart(const sol::function& Func);
    void Lua_SetOnUpdate(const sol::function& Func);
    void Lua_SetOnAbort(const sol::function& Func);
    void Lua_SetOnComplete(const sol::function& Func);
    //


    EvaluateUtilityFunc EvaluateUtility = nullptr;

    OnDefaultFunc OnPatternStart = nullptr;
    OnUpdateFunc OnPatternUpdate = nullptr;
    OnDefaultFunc OnPatternAbort = nullptr;
    OnCompleteFunc OnPatternComplete = nullptr;

private:
    EActionCondition Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void AbortCurrentAction();

    void Reset();


    eastl::vector<eastl::shared_ptr<Action>> Actions;

    size_t CurrentActionIndex = 0;

    bool bStarted = false;
};


class ConditionTransition
{
public:
    explicit ConditionTransition(const std::string& InToState, CheckFunc InCheck) : ToState(InToState), Check(InCheck) { }


    CheckFunc Check = nullptr;

    std::string ToState;
};


class EventTransition
{
public:
    explicit EventTransition(const std::string& InToState, CheckFunc InCheck, BehaviorController* FSM);

    void Trigger(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);

    void ChangeToState(const std::string& InToState, BehaviorController* FSM);


    CheckFunc Check = nullptr;

    AbortFunc Abort = nullptr;

    std::string ToState;
};


class State
{
    friend class BehaviorController;

public:
    State() = default;

    bool AddPattern(const std::string& Name, eastl::shared_ptr<Pattern> Pattern);
    eastl::shared_ptr<Pattern> GetPattern(const std::string& Name);
    bool RemovePattern(const std::string& Name);

    eastl::hash_map<std::string, eastl::shared_ptr<Pattern>> GetAllPatternsCopy() { return Patterns; };

    // --- Lua friendly ---
    sol::table Lua_GetAllPatterns(sol::this_state ts) const;

    void Lua_SetOnEnter(const sol::function& Func);
    void Lua_SetOnUpdate(const sol::function& Func);
    void Lua_SetOnAbort(const sol::function& Func);
    void Lua_SetOnExit(const sol::function& Func);
    //


    OnDefaultFunc OnStateEnter = nullptr;
    OnUpdateFunc OnStateUpdate = nullptr;
    OnDefaultFunc OnStateAbort = nullptr;
    OnDefaultFunc OnStateExit = nullptr;

private:
    void AddConditionTransition(const std::string& InToState, CheckFunc InCheck);
    void AddEventTransition(const std::string& InToState, CheckFunc InCheck, BehaviorController* FSM);

    bool RemoveConditionTransition(const std::string& ToState);
    bool RemoveEventTransition(const std::string& ToState);

    bool Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    eastl::shared_ptr<ConditionTransition> CheckConditionTransitions(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard);


    eastl::vector<eastl::shared_ptr<ConditionTransition>> ConditionTransitions;
    eastl::vector<eastl::shared_ptr<EventTransition>> EventTransitions;

    eastl::hash_map<std::string, eastl::shared_ptr<Pattern>> Patterns;
    eastl::shared_ptr<Pattern> CurrentPattern = nullptr;

    bool IsRunning = false;
};


class BehaviorController : public Component
{
    friend class Game;
    friend class EventTransition;

public:
    BehaviorController(const SE::UUID& InGOID) : GOID(InGOID) { MBoard = eastl::shared_ptr<MemoryBoard>(new MemoryBoard()); };

    // --- MemoryBoard ---
    void SetMemoryBoard(const eastl::shared_ptr<MemoryBoard>& InMemoryBoard);
    eastl::shared_ptr<MemoryBoard> GetMemoryBoard() { return MBoard; };
    //

    // --- States ---
    bool AddState(const std::string& Name, const eastl::shared_ptr<State>& NewState);
    eastl::shared_ptr<State> GetState(const std::string& Name);
    bool RemoveState(const std::string& Name);

    bool SetInitialState(const std::string& Name);
    const std::string& GetCurrentStateName() const { return CurrentStateName; }

    eastl::hash_map<std::string, eastl::shared_ptr<State>> GetAllStatesCopy() const { return States; }
    //

    // --- Conditions ---
    bool AddConditionTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck);
    bool AddEventTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck);

    bool ChangeToStateInConditionTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState);
    bool ChangeToStateInEventTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState);

    bool ChangeCheckFuncInConditionTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck);
    bool ChangeCheckFuncInEventTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck);

    bool RemoveConditionTransition(const std::string& FromState, const std::string& ToState);
    bool RemoveEventTransition(const std::string& FromState, const std::string& ToState);
    //


    // LUA
    sol::table Lua_GetAllStates(sol::this_state ts);


    bool IsEnabled = true;

private:
    void Update(float DeltaTime);

    void Abort(const std::string& ToState);

    void ChangeState(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const std::string& NewState);


    SE::UUID GOID;
    eastl::shared_ptr<MemoryBoard> MBoard = nullptr;

    eastl::hash_map<std::string, eastl::shared_ptr<State>> States;
    std::string CurrentStateName;
    eastl::shared_ptr<State> CurrentState = nullptr;

    std::string AfterAbortStateName;
    bool IsClosedForAbort = false;
};


class BehaviorController_Info : public Component_Info
{
public:
    BehaviorController_Info() {};

    static const SE::ComponentType s_componentType = SE::ComponentType::BEHAVIOR;

    const SE::ComponentType ComponentType() const override { return s_componentType; }
    const std::type_info& getType() const override { return typeid(BehaviorController_Info); }
    bool IsAssigned() override { return true; }


    BehaviorController* Controller;
};



// --- LUA BINDING ---
#ifndef ACTION_LUA_METHODS_APPLY
#define ACTION_LUA_METHODS_APPLY(FM) \
    FM("getName",        &Action::GetName) , \
    FM("setOnStart",     &Action::Lua_SetOnStart) , \
    FM("setOnUpdate",    &Action::Lua_SetOnUpdate) , \
    FM("setOnAbort",     &Action::Lua_SetOnAbort) , \
    FM("setOnComplete",  &Action::Lua_SetOnComplete)
#endif



#ifndef PATTERN_LUA_METHODS_APPLY
#define PATTERN_LUA_METHODS_APPLY(FM) \
    FM("addAction",            &Pattern::AddAction) , \
    FM("insertAction",         &Pattern::InsertAction) , \
    FM("getActionByName",      &Pattern::GetActionByName) , \
    FM("getActionByIndex",     &Pattern::GetActionByIndex) , \
    FM("removeActionByName",   &Pattern::RemoveActionByName) , \
    FM("removeActionByIndex",  &Pattern::RemoveActionByIndex) , \
    FM("getAllActions",        &Pattern::GetAllActions) , \
    FM("setEvaluateUtility",   &Pattern::Lua_SetEvaluateUtility) , \
    FM("setOnStart",           &Pattern::Lua_SetOnStart) , \
    FM("setOnUpdate",          &Pattern::Lua_SetOnUpdate) , \
    FM("setOnAbort",           &Pattern::Lua_SetOnAbort) , \
    FM("setOnComplete",        &Pattern::Lua_SetOnComplete)
#endif



#ifndef STATE_LUA_METHODS_APPLY
#define STATE_LUA_METHODS_APPLY(FM) \
    FM("addPattern",         &State::AddPattern) , \
    FM("getPattern",         &State::GetPattern) , \
    FM("removePattern",      &State::RemovePattern) , \
    FM("getAllPatterns",     &State::Lua_GetAllPatterns) , \
    FM("setOnEnter",         &State::Lua_SetOnEnter) , \
    FM("setOnUpdate",        &State::Lua_SetOnUpdate) , \
    FM("setOnAbort",         &State::Lua_SetOnAbort) , \
    FM("setOnExit",          &State::Lua_SetOnExit)
#endif



#ifndef BEHAVIORCONTROLLER_LUA_FIELDS_APPLY
#define BEHAVIORCONTROLLER_LUA_FIELDS_APPLY(F) \
    F(IsEnabled)
#endif

#ifndef BEHAVIORCONTROLLER_LUA_METHODS_APPLY
#define BEHAVIORCONTROLLER_LUA_METHODS_APPLY(FM) \
    FM("setMemoryBoard",                 &BehaviorController::SetMemoryBoard) , \
    FM("getMemoryBoard",                 &BehaviorController::GetMemoryBoard) , \
    FM("addState",                       &BehaviorController::AddState) , \
    FM("getState",                       &BehaviorController::GetState) , \
    FM("removeState",                    &BehaviorController::RemoveState) , \
    FM("setInitialState",                &BehaviorController::SetInitialState) , \
    FM("getCurrentStateName",            &BehaviorController::GetCurrentStateName) , \
    FM("getAllStates",                   &BehaviorController::Lua_GetAllStates) , \
    FM("addConditionTransition",         &BehaviorController::AddConditionTransition) , \
    FM("addEventTransition",             &BehaviorController::AddEventTransition) , \
    FM("changeConditionToState",         &BehaviorController::ChangeToStateInConditionTransition) , \
    FM("changeEventToState",             &BehaviorController::ChangeToStateInEventTransition) , \
    FM("changeConditionCheck",           &BehaviorController::ChangeCheckFuncInConditionTransition) , \
    FM("changeEventCheck",               &BehaviorController::ChangeCheckFuncInEventTransition) , \
    FM("removeConditionTransition",      &BehaviorController::RemoveConditionTransition) , \
    FM("removeEventTransition",          &BehaviorController::RemoveEventTransition)
#endif