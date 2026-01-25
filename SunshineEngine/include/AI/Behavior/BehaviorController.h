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


enum class EActionResult
{
    Running,    // 0
    Succeeded,  // 1
    Failed      // 2
};

enum class EActionCondition
{
    Running, 
    Succeeded,
    Failed,
    Aborted 
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
    friend class BehaviorController;

public:
    // --- General ---
    explicit Action(const std::string& InName) : Name(InName) {}

    const std::string& GetName() const { return Name; }
    void SetName(const std::string& NewName) { Name = NewName; }
    //
private:
    // --- In game ---
    EActionCondition Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime);
    void Abort() { IsAborted = true; }
    //

    std::string Name;

    sol::function OnActionStart;
    sol::function OnActionUpdate;
    sol::function OnActionAbort;
    sol::function OnActionComplete;

    bool IsAborted = false;
};


class Pattern
{
    friend class BehaviorController;
    friend class State;

public:
    Pattern() = default;

    // --- Actions ---
    std::shared_ptr<Action> Action_GetByName(const std::string& Name) const;
    std::shared_ptr<Action> Action_GetByIndex(size_t Index) const;

    bool Action_Add(const std::string& Name);
    bool Action_Insert(const std::string& Name, size_t Index);
    sol::table Action_GetAll(sol::this_state L);

    bool Action_RemoveByName(const std::string& Name);
    bool Action_RemoveByIndex(size_t Index);
    //
private:
    // --- In game ---
    EActionCondition Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime);
    void AbortCurrentAction();
    void Reset();
    //


    std::vector<std::shared_ptr<Action>> Actions;

    size_t CurrentActionIndex = 0;

    bool bStarted = false;

    sol::function EvaluateUtility;

    sol::function OnPatternStart;
    sol::function OnPatternUpdate;
    sol::function OnPatternAbort;
    sol::function OnPatternComplete;
};


class ConditionTransition
{
public:
    explicit ConditionTransition(const std::string& InToState, sol::function InCheck) : ToState(InToState), Check(InCheck) {}


    sol::function Check;

    std::string ToState;
};


class State
{
    friend class BehaviorController;

public:
    State() = default;

    // --- Patterns ---
    std::shared_ptr<Pattern> Pattern_Get(const std::string& Name);

    bool Pattern_Add(const std::string& Name);
    bool Pattern_Remove(const std::string& Name);
    sol::table Pattern_GetAll(sol::this_state L) const;
    //
private:
    // --- Transition Conditions ---
    void ConditionTransition_Add(const std::string& InToState, sol::function InCheck);
    bool ConditionTransition_Remove(const std::string& ToState);
    //
    
    // --- In game ---
    bool Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime);
    std::shared_ptr<ConditionTransition> CheckConditionTransitions(const SE::UUID& GOID, BehaviorController* BC);
    //


    std::vector<std::shared_ptr<ConditionTransition>> ConditionTransitions;

    std::unordered_map<std::string, std::shared_ptr<Pattern>> Patterns;
    std::shared_ptr<Pattern> CurrentPattern = nullptr;

    bool IsRunning = false;

    sol::function OnStateEnter;
    sol::function OnStateUpdate;
    sol::function OnStateAbort;
    sol::function OnStateExit;
};


class BehaviorController : public Component
{
    friend class BehaviorStorage;

public:
    // --- General ---
    explicit BehaviorController(const SE::UUID& InGOID) : GOID(InGOID)
    {
        BehaviorStorage::Get().AddBehavior(this);
        MBoard = std::make_shared<MemoryBoard>();
    };

    ~BehaviorController()
    {
        BehaviorStorage::Get().RemoveBehavior(this);
        MBoard->Clear();
    }

    void SetIsEnabled(bool NewCondition) { IsEnabled = NewCondition; }
    bool GetIsEnabled() { return IsEnabled; }
    sol::table GetAllStates(sol::this_state L) const;

    void Trigger(const std::string& ToState);

    void FromJson(const json& j) override;

    static const SE::ComponentType s_componentType = SE::ComponentType::BEHAVIOR;
    const std::type_info& getType() const override { return typeid(BehaviorController); }
    const SE::ComponentType ComponentType() const override { return s_componentType; }
    //

    // --- MemoryBoard ---
    bool MemoryBoard_SetInt(const std::string& Key, int Value) { return MBoard->SetTypedValue<int>(Key, Value); }
    bool MemoryBoard_SetFloat(const std::string& Key, float Value) { return MBoard->SetTypedValue<float>(Key, Value); }
    bool MemoryBoard_SetBool(const std::string& Key, bool Value) { return MBoard->SetTypedValue<bool>(Key, Value); }
    bool MemoryBoard_SetString(const std::string& Key, const std::string& Value) { return MBoard->SetTypedValue<std::string>(Key, Value); }
    bool MemoryBoard_SetVector3(const std::string& Key, const DXSM::Vector3& Value) { return MBoard->SetTypedValue<DXSM::Vector3>(Key, Value); }
    bool MemoryBoard_SetUUID(const std::string& Key, const SE::UUIDhilo Value) { return MBoard->SetTypedValue<SE::UUID>(Key, SE::UUID::FromHilo(Value)); }

    sol::object MemoryBoard_GetInt(const std::string& Key, sol::this_state L) const;
    sol::object MemoryBoard_GetFloat(const std::string& Key, sol::this_state L) const;
    sol::object MemoryBoard_GetBool(const std::string& Key, sol::this_state L) const;
    sol::object MemoryBoard_GetString(const std::string& Key, sol::this_state L) const;
    sol::object MemoryBoard_GetVector3(const std::string& Key, sol::this_state L) const;
    sol::object MemoryBoard_GetUUID(const std::string& Key, sol::this_state L) const;

    bool MemoryBoard_HasKey(const std::string& Key) const { return MBoard->HasKey(Key); }
    void MemoryBoard_RemoveKey(const std::string& Key) { MBoard->RemoveKey(Key); }
    void MemoryBoard_Clear() { MBoard->Clear(); }

    uint64_t MemoryBoard_AddCallback(const std::string& Key, const sol::function& Callback) { return MBoard->AddCallback(Key, Callback);};
    void MemoryBoard_RemoveCallback(const std::string& Key, uint64_t CallbackId) { MBoard->RemoveCallback(Key, CallbackId); };
    void MemoryBoard_ClearCallbacks(const std::string& Key) { MBoard->ClearCallbacks(Key); }
    //

    // --- States ---
    std::shared_ptr<State> State_Get(const std::string& Name);

    bool State_Add(const std::string& Name);
    bool State_Remove(const std::string& Name);
    bool State_SetInitial(const std::string& Name);
    const std::string& State_GetCurrent() const { return CurrentStateName; }
    sol::table State_GetAllPatterns(const std::string& Name, sol::this_state L);

    bool State_SetOnEnter(const std::string& Name, const sol::function& Func);
    bool State_SetOnUpdate(const std::string& Name, const sol::function& Func);
    bool State_SetOnAbort(const std::string& Name, const sol::function& Func);
    bool State_SetOnExit(const std::string& Name, const sol::function& Func);
    //

    // --- Transition Conditions ---
    bool ConditionTransition_Add(const std::string& FromState, const std::string& ToState, sol::function InCheck);
    bool ConditionTransition_Has(const std::string& FromState, const std::string& ToState);
    bool ConditionTransition_ChangeToState(const std::string& FromState, const std::string& OldToState, const std::string& NewToState);
    bool ConditionTransition_ChangeCheckFunc(const std::string& FromState, const std::string& ToState, sol::function InCheck);
    bool ConditionTransition_Remove(const std::string& FromState, const std::string& ToState);
    //

    // --- Patterns ---
    bool Pattern_Add(const std::string& NameS, const std::string& NameP);
    bool Pattern_Remove(const std::string& NameS, const std::string& NameP);
    sol::table Pattern_GetAllActions(const std::string& NameS, const std::string& NameP, sol::this_state L);
    int Pattern_Count(const std::string& NameS, const std::string& NameP);

    bool Pattern_SetEvaluateUtility(const std::string& NameS, const std::string& NameP, const sol::function& Func);
    bool Pattern_SetOnStart(const std::string& NameS, const std::string& NameP, const sol::function& Func);
    bool Pattern_SetOnUpdate(const std::string& NameS, const std::string& NameP, const sol::function& Func);
    bool Pattern_SetOnAbort(const std::string& NameS, const std::string& NameP, const sol::function& Func);
    bool Pattern_SetOnComplete(const std::string& NameS, const std::string& NameP, const sol::function& Func);
    //

    // --- Actions ---
    bool Action_Add(const std::string& NameS, const std::string& NameP, const std::string& NameA);
    bool Action_Insert(const std::string& NameS, const std::string& NameP, const std::string& NameA, size_t Index);
    bool Action_RemoveByName(const std::string& NameS, const std::string& NameP, const std::string& NameA);
    bool Action_RemoveByIndex(const std::string& NameS, const std::string& NameP, size_t Index);

    bool Action_SetOnStart(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func);
    bool Action_SetOnUpdate(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func);
    bool Action_SetOnAbort(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func);
    bool Action_SetOnComplete(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func);
    //

private:
    // --- In game ---
    void Update(float DeltaTime);
    void Abort(const std::string& ToState);
    void ChangeState(const SE::UUID& GOID, const std::string& NewState);
    //


    SE::UUID GOID;
    std::shared_ptr<MemoryBoard> MBoard;

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

#ifndef BEHAVIORCONTROLLER_LUA_METHODS_APPLY
#define BEHAVIORCONTROLLER_LUA_METHODS_APPLY(FM) \
    FM("setIsEnabled",                   &BehaviorController::SetIsEnabled), \
    FM("getIsEnabled",                   &BehaviorController::GetIsEnabled), \
    FM("getAllStates",                   &BehaviorController::GetAllStates), \
    FM("trigger",                        &BehaviorController::Trigger), \
    \
    FM("MB_setInt",                      &BehaviorController::MemoryBoard_SetInt), \
    FM("MB_setFloat",                    &BehaviorController::MemoryBoard_SetFloat), \
    FM("MB_setBool",                     &BehaviorController::MemoryBoard_SetBool), \
    FM("MB_setString",                   &BehaviorController::MemoryBoard_SetString), \
    FM("MB_setVector3",                  &BehaviorController::MemoryBoard_SetVector3), \
    FM("MB_setUUID",                     &BehaviorController::MemoryBoard_SetUUID), \
    \
    FM("MB_getInt",                      &BehaviorController::MemoryBoard_GetInt), \
    FM("MB_getFloat",                    &BehaviorController::MemoryBoard_GetFloat), \
    FM("MB_getBool",                     &BehaviorController::MemoryBoard_GetBool), \
    FM("MB_getString",                   &BehaviorController::MemoryBoard_GetString), \
    FM("MB_getVector3",                  &BehaviorController::MemoryBoard_GetVector3), \
    FM("MB_getUUID",                     &BehaviorController::MemoryBoard_GetUUID), \
    \
    FM("MB_hasKey",                      &BehaviorController::MemoryBoard_HasKey), \
    FM("MB_removeKey",                   &BehaviorController::MemoryBoard_RemoveKey), \
    FM("MB_clear",                       &BehaviorController::MemoryBoard_Clear), \
    \
    FM("MB_addCallback",                 &BehaviorController::MemoryBoard_AddCallback) , \
    FM("MB_removeCallback",              &BehaviorController::MemoryBoard_RemoveCallback), \
    FM("MB_clearCallbacks",              &BehaviorController::MemoryBoard_ClearCallbacks), \
    \
    FM("S_add",                          &BehaviorController::State_Add) , \
    FM("S_remove",                       &BehaviorController::State_Remove) , \
    FM("S_setInitial",                   &BehaviorController::State_SetInitial), \
    FM("S_getCurrent",                   &BehaviorController::State_GetCurrent), \
    FM("S_getAllPatterns",               &BehaviorController::State_GetAllPatterns), \
    FM("S_setOnEnter",                   &BehaviorController::State_SetOnEnter), \
    FM("S_setOnUpdate",                  &BehaviorController::State_SetOnUpdate), \
    FM("S_setOnAbort",                   &BehaviorController::State_SetOnAbort), \
    FM("S_setOnExit",                    &BehaviorController::State_SetOnExit), \
    \
    FM("ET_Trigger",                     &BehaviorController::State_SetOnExit), \
    \
    FM("CT_add",                         &BehaviorController::ConditionTransition_Add) , \
    FM("CT_has",                         &BehaviorController::ConditionTransition_Has) , \
    FM("CT_changeToState",               &BehaviorController::ConditionTransition_ChangeToState) , \
    FM("CT_changeCheckFunc",             &BehaviorController::ConditionTransition_ChangeCheckFunc), \
    FM("CT_remove",                      &BehaviorController::ConditionTransition_Remove) , \
    \
    FM("P_add",                          &BehaviorController::Pattern_Add), \
    FM("P_remove",                       &BehaviorController::Pattern_Remove), \
    FM("P_getAllActions",                &BehaviorController::Pattern_GetAllActions), \
    FM("P_count",                        &BehaviorController::Pattern_Count), \
    FM("P_setEvaluateUtility",           &BehaviorController::Pattern_SetEvaluateUtility), \
    FM("P_setOnStart",                   &BehaviorController::Pattern_SetOnStart), \
    FM("P_setOnUpdate",                  &BehaviorController::Pattern_SetOnUpdate), \
    FM("P_setOnAbort",                   &BehaviorController::Pattern_SetOnAbort), \
    FM("P_setOnComplete",                &BehaviorController::Pattern_SetOnComplete), \
    \
    FM("A_add",                          &BehaviorController::Action_Add), \
    FM("A_insert",                       &BehaviorController::Action_Insert), \
    FM("A_removeByName",                 &BehaviorController::Action_RemoveByName), \
    FM("A_removeByIndex",                &BehaviorController::Action_RemoveByIndex), \
    FM("A_setOnStart",                   &BehaviorController::Action_SetOnStart), \
    FM("A_setOnUpdate",                  &BehaviorController::Action_SetOnUpdate), \
    FM("A_setOnAbort",                   &BehaviorController::Action_SetOnAbort), \
    FM("A_setOnComplete",                &BehaviorController::Action_SetOnComplete)
#endif
