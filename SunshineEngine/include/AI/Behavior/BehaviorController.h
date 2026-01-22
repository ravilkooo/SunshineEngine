#pragma once

// Engine
#include "MemoryBoard.h"
#include "AI/Behavior/BehaviorStorage.h"
#include <Utils/UUID.h>
#include <Component/Component.h>

// Eastl
#include <EASTL/unique_ptr.h>

// C++
#include <vector>
#include <memory>
#include <unordered_map>

// Lua
#include <sol/sol.hpp>

// Json
#include <nlohmann/json.hpp>


using json = nlohmann::json;


enum class EActionCondition
{
    Running,   // 0
    Succeeded, // 1
    Failed,    // 2
    Aborted    // 3
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


using AbortFunc = std::function<void(const std::string& ToState)>;



class Action
{
    friend class Pattern;

public:
    explicit Action(const std::string& InName) : Name(InName) {}

    const std::string& GetName() const { return Name; }
    void SetName(const std::string& NewName) { Name = NewName; }

    void SetOnStart(const sol::function& Func) { OnActionStart = Func; }
    void SetOnUpdate(const sol::function& Func) { OnActionUpdate = Func; };
    void SetOnAbort(const sol::function& Func) { OnActionAbort = Func; };
    void SetOnComplete(const sol::function& Func) { OnActionComplete = Func; };

private:
    EActionCondition Update(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void Abort() { IsAborted = true; }


    std::string Name;

    sol::function OnActionStart = nullptr;
    sol::function OnActionUpdate = nullptr;
    sol::function OnActionAbort = nullptr;
    sol::function OnActionComplete = nullptr;

    bool IsAborted = false;
};


class Pattern
{
    friend class BehaviorController;
    friend class State;

public:
    Pattern() = default;

    void AddAction(std::shared_ptr<Action> NewAction);
    void InsertAction(std::shared_ptr<Action> NewAction, size_t Index);

    std::shared_ptr<Action> GetActionByName(const std::string& Name) const;
    std::shared_ptr<Action> GetActionByIndex(size_t Index) const;

    bool RemoveActionByName(const std::string& Name);
    bool RemoveActionByIndex(size_t Index);

    std::vector<std::shared_ptr<Action>> GetAllActions() const { return Actions; }

    void SetEvaluateUtility(const sol::function& Func) { EvaluateUtility = Func; }

    void SetOnStart(const sol::function& Func) { OnPatternStart = Func; }
    void SetOnUpdate(const sol::function& Func) { OnPatternUpdate = Func; }
    void SetOnAbort(const sol::function& Func) { OnPatternAbort = Func; }
    void SetOnComplete(const sol::function& Func) { OnPatternComplete = Func; }

private:
    EActionCondition Update(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void AbortCurrentAction();

    void Reset();


    std::vector<std::shared_ptr<Action>> Actions;

    size_t CurrentActionIndex = 0;

    bool bStarted = false;

    sol::function EvaluateUtility = nullptr;

    sol::function OnPatternStart = nullptr;
    sol::function OnPatternUpdate = nullptr;
    sol::function OnPatternAbort = nullptr;
    sol::function OnPatternComplete = nullptr;
};


class ConditionTransition
{
public:
    explicit ConditionTransition(const std::string& InToState, sol::function InCheck) : ToState(InToState), Check(InCheck) {}


    sol::function Check = nullptr;

    std::string ToState;
};


class EventTransition
{
public:
    explicit EventTransition(const std::string& InToState, sol::function InCheck, BehaviorController* FSM);

    void Trigger(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard);

    void ChangeToState(const std::string& InToState, BehaviorController* FSM);


    sol::function Check = nullptr;

    AbortFunc Abort = nullptr;

    std::string ToState;
};


class State
{
    friend class BehaviorController;

public:
    State() = default;

    bool AddPattern(const std::string& Name, std::shared_ptr<Pattern> NewPattern);
    std::shared_ptr<Pattern> GetPattern(const std::string& Name);
    bool RemovePattern(const std::string& Name);

    const std::unordered_map<std::string, std::shared_ptr<Pattern>>& GetAllPatterns() { return Patterns; };

    sol::table Lua_GetAllPatterns(sol::this_state L) const
    {
        sol::state_view lua(L);
        sol::table t = lua.create_table();

        for (auto& [k, v] : Patterns)
            t[k] = v;

        return t;
    }

    void SetOnEnter(const sol::function& Func) { OnStateEnter = Func; }
    void SetOnUpdate(const sol::function& Func) { OnStateUpdate = Func; }
    void SetOnAbort(const sol::function& Func) { OnStateAbort = Func; }
    void SetOnExit(const sol::function& Func) { OnStateExit = Func; }

private:
    void AddConditionTransition(const std::string& InToState, sol::function InCheck);
    void AddEventTransition(const std::string& InToState, sol::function InCheck, BehaviorController* FSM);

    bool RemoveConditionTransition(const std::string& ToState);
    bool RemoveEventTransition(const std::string& ToState);

    bool Update(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    std::shared_ptr<ConditionTransition> CheckConditionTransitions(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard);


    std::vector<std::shared_ptr<ConditionTransition>> ConditionTransitions;
    std::vector<std::shared_ptr<EventTransition>> EventTransitions;

    std::unordered_map<std::string, std::shared_ptr<Pattern>> Patterns;
    std::shared_ptr<Pattern> CurrentPattern = nullptr;

    bool IsRunning = false;

    sol::function OnStateEnter = nullptr;
    sol::function OnStateUpdate = nullptr;
    sol::function OnStateAbort = nullptr;
    sol::function OnStateExit = nullptr;
};


class BehaviorController : public Component
{
    friend class BehaviorStorage;
    friend class EventTransition;
    friend class BehaviorController_Info;

public:
    explicit BehaviorController(const SE::UUID& InGOID) : GOID(InGOID) { BehaviorStorage::Get().AddBehavior(this); };

    ~BehaviorController() { BehaviorStorage::Get().RemoveBehavior(this); }

    // --- MemoryBoard ---
    void SetMemoryBoard(std::shared_ptr<MemoryBoard>& InMemoryBoard);
    std::shared_ptr<MemoryBoard> GetMemoryBoard() { return MBoard; };
    //

    // --- States ---
    bool AddState(const std::string& Name, const std::shared_ptr<State>& NewState);
    std::shared_ptr<State> GetState(const std::string& Name);
    bool RemoveState(const std::string& Name);

    bool SetInitialState(const std::string& Name);
    const std::string& GetCurrentStateName() const { return CurrentStateName; }

    std::unordered_map<std::string, std::shared_ptr<State>> GetAllStates() const { return States; }

    sol::table Lua_GetAllStates(sol::this_state L) const
    {
        sol::state_view lua(L);
        sol::table t = lua.create_table();

        for (const auto& [name, state] : States)
        {
            t[name] = state;
        }

        return t;
    }
    //

    // --- Conditions ---
    bool AddConditionTransition(const std::string& FromState, const std::string& ToState, sol::function InCheck);
    bool AddEventTransition(const std::string& FromState, const std::string& ToState, sol::function InCheck);

    bool ChangeToStateInConditionTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState);
    bool ChangeToStateInEventTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState);

    bool ChangeCheckFuncInConditionTransition(const std::string& FromState, const std::string& ToState, sol::function InCheck);
    bool ChangeCheckFuncInEventTransition(const std::string& FromState, const std::string& ToState, sol::function InCheck);

    bool RemoveConditionTransition(const std::string& FromState, const std::string& ToState);
    bool RemoveEventTransition(const std::string& FromState, const std::string& ToState);
    //

    void SetIsEnabled(bool NewCondition) { IsEnabled = NewCondition; }
    bool GetIsEnabled() { return IsEnabled; }

    const std::type_info& getType() const override { return typeid(BehaviorController); }

    static const SE::ComponentType s_componentType = SE::ComponentType::BEHAVIOR;
    const SE::ComponentType ComponentType() const override { return s_componentType; }

    void FromJson(const json& j) override;

private:
    void Update(float DeltaTime);

    void Abort(const std::string& ToState);

    void ChangeState(const SE::UUID& GOID, std::shared_ptr<MemoryBoard>& MBoard, const std::string& NewState);


    SE::UUID GOID;
    std::shared_ptr<MemoryBoard> MBoard = nullptr;

    std::unordered_map<std::string, std::shared_ptr<State>> States;
    std::string CurrentStateName;
    std::shared_ptr<State> CurrentState = nullptr;

    std::string AfterAbortStateName;
    bool IsClosedForAbort = false;

    bool IsEnabled = false;
};


class BehaviorController_Info : public Component_Info
{
public:
    explicit BehaviorController_Info() { }

    static const SE::ComponentType s_componentType = SE::ComponentType::BEHAVIOR;

    const SE::ComponentType ComponentType() const override { return s_componentType; }
    const std::type_info& getType() const override { return typeid(BehaviorController_Info); }
    bool IsAssigned() override { return true; }

    json ToJson() const override;
    void FromJson(const json& j) override;

    bool IsEnabled = false;
};


// --- LUA BINDING ---

#ifndef ACTION_LUA_METHODS_APPLY
#define ACTION_LUA_METHODS_APPLY(FM) \
    FM("getName",        &Action::GetName) , \
    FM("setName",        &Action::SetName) , \
    FM("setOnStart",     &Action::SetOnStart) , \
    FM("setOnUpdate",    &Action::SetOnUpdate) , \
    FM("setOnAbort",     &Action::SetOnAbort) , \
    FM("setOnComplete",  &Action::SetOnComplete)
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
    FM("setEvaluateUtility",   &Pattern::SetEvaluateUtility) , \
    FM("setOnStart",           &Pattern::SetOnStart) , \
    FM("setOnUpdate",          &Pattern::SetOnUpdate) , \
    FM("setOnAbort",           &Pattern::SetOnAbort) , \
    FM("setOnComplete",        &Pattern::SetOnComplete)
#endif



#ifndef STATE_LUA_METHODS_APPLY
#define STATE_LUA_METHODS_APPLY(FM) \
    FM("addPattern",         &State::AddPattern) , \
    FM("getPattern",         &State::GetPattern) , \
    FM("removePattern",      &State::RemovePattern) , \
    FM("getAllPatterns",     &State::Lua_GetAllPatterns) , \
    FM("setOnEnter",         &State::SetOnEnter) , \
    FM("setOnUpdate",        &State::SetOnUpdate) , \
    FM("setOnAbort",         &State::SetOnAbort) , \
    FM("setOnExit",          &State::SetOnExit)
#endif



#ifndef BEHAVIORCONTROLLER_LUA_METHODS_APPLY
#define BEHAVIORCONTROLLER_LUA_METHODS_APPLY(FM) \
    FM("setIsEnabled",                   &BehaviorController::SetIsEnabled) , \
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
