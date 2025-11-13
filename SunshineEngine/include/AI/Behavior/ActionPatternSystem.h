#pragma once

#include <EASTL/string.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/optional.h>
#include <EASTL/utility.h>

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


class Action
{
    friend class Pattern;

public:
    explicit Action(const eastl::string& InName) : Name(InName) {}

    const eastl::string& GetName() const { return Name; }


    OnDefaultFunc OnActionStart = nullptr;
    OnActionUpdateFunc OnActionUpdate = nullptr;
    OnDefaultFunc OnActionAbort = nullptr;
    OnCompleteFunc OnActionComplete = nullptr;

private:
    EActionCondition Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);

    void Abort() { IsAborted = true; }


    eastl::string Name;

    bool IsAborted = false;
};


class Pattern
{
    friend class FiniteStateMachine;
    friend class ActionPatternSystem;

public:
    Pattern() {}

    void AddAction(eastl::shared_ptr<Action> NewAction);
    void InsertAction(eastl::shared_ptr<Action> NewAction, size_t Index);


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


class ActionPatternSystem
{
    friend class FiniteStateMachine;
    friend class State;

public:
    void AddPattern(const eastl::string& Name, eastl::shared_ptr<Pattern> Pattern);
    void RemovePattern(const eastl::string& PatternName);

private:
    EStateResult Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime);


    eastl::hash_map<eastl::string, eastl::shared_ptr<Pattern>> Patterns;

    eastl::shared_ptr<Pattern> CurrentPattern = nullptr;
};
