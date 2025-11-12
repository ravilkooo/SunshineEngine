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


class Action
{
    friend class FiniteStateMachine;

public:
    explicit Action(const eastl::string& InName) : Name(InName) {}

    const eastl::string& GetName() const { return Name; }

    EActionCondition Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime);

    void Abort() { IsAborted = true; }

    //
    virtual void OnActionStart(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual EActionResult OnActionUpdate(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime) { return EActionResult::Succeeded; }
    virtual void OnActionAbort(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual void OnActionComplete(const Sunshine::UUID& GOID, MemoryBoard* MBoard, EActionResult Result) {}

protected:
    eastl::string Name;

    bool IsAborted = false;
};


class Pattern
{
    friend class FiniteStateMachine;

public:
    Pattern() {}

    void AddAction(eastl::shared_ptr<Action> NewAction);
    void InsertAction(eastl::shared_ptr<Action> NewAction, size_t Index);

    EActionCondition Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime);

    void AbortCurrentAction();

    void Reset();

    //
    virtual float EvaluateUtility(MemoryBoard* MBoard) { return 0.0f; }

    virtual void OnPatternStart(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual void OnPatternUpdate(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime) {};
    virtual void OnPatternAbort(const Sunshine::UUID& GOID, MemoryBoard* MBoard) {}
    virtual void OnPatternComplete(const Sunshine::UUID& GOID, MemoryBoard* MBoard, EActionResult Result) {}

private:
    eastl::vector<eastl::shared_ptr<Action>> Actions;

    size_t CurrentActionIndex = 0;

    bool bStarted = false;
};


class ActionPatternSystem
{
    friend class FiniteStateMachine;

public:
    void AddPattern(const eastl::string& Name, eastl::shared_ptr<Pattern> Pattern);
    void RemovePattern(const eastl::string& PatternName);

    EStateResult Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime);

private:
    eastl::hash_map<eastl::string, eastl::shared_ptr<Pattern>> Patterns;

    eastl::shared_ptr<Pattern> CurrentPattern = nullptr;
};
