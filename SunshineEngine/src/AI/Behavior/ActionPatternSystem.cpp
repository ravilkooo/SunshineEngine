#include "AI/Behavior/ActionPatternSystem.h"

#include <EASTL/numeric_limits.h>
#include <EASTL/algorithm.h>


// ----------------- Action -----------------

EActionCondition Action::Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime)
{
    if (IsAborted == true)
    {
        OnActionAbort(GOID, MBoard);
        IsAborted = false;

        return EActionCondition::Aborted;
    }

    switch (OnActionUpdate(GOID, MBoard, DeltaTime))
    {
        case EActionResult::Succeeded:
            OnActionComplete(GOID, MBoard, EActionResult::Succeeded);

            return EActionCondition::Succeeded;

        case EActionResult::Failed:
            OnActionComplete(GOID, MBoard, EActionResult::Failed);

            return EActionCondition::Failed;

        case EActionResult::Running:  
            return EActionCondition::Running;
    }
}


// ----------------- Pattern -----------------

void Pattern::AddAction(eastl::shared_ptr<Action> NewAction)
{
    if (NewAction)
        Actions.push_back(NewAction);
    else
        std::cerr << "[Warning] " << "\n";
}


void Pattern::InsertAction(eastl::shared_ptr<Action> NewAction, size_t Index)
{
    if (!NewAction)
    {
        std::cerr << "[Warning] " << "\n";
        return;
    }

    if (Index >= Actions.size())
        Actions.push_back(NewAction);
    else
        Actions.insert(Actions.begin() + Index, NewAction);
}

EActionCondition Pattern::Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime)
{
    OnPatternUpdate(GOID, MBoard, DeltaTime);

    if (Actions.empty())
        return EActionCondition::Succeeded;

    if (!bStarted)
    {
        Actions[CurrentActionIndex]->OnActionStart(GOID, MBoard);

        bStarted = true;
    }

    switch (Actions[CurrentActionIndex]->Update(GOID, MBoard, DeltaTime))
    {
        case EActionCondition::Succeeded:

            ++CurrentActionIndex;

            if (CurrentActionIndex >= Actions.size())
            {
                Reset();
                OnPatternComplete(GOID, MBoard, EActionResult::Succeeded);

                return EActionCondition::Succeeded;
            }
            else
            {
                bStarted = false;

                return EActionCondition::Running;
            }

        case EActionCondition::Failed:

            Reset();
            OnPatternComplete(GOID, MBoard, EActionResult::Failed);

            return EActionCondition::Failed;

        case EActionCondition::Running:

            return EActionCondition::Running;

        case EActionCondition::Aborted:

            Reset();
            OnPatternAbort(GOID, MBoard);

            return EActionCondition::Aborted;
    }
}

void Pattern::AbortCurrentAction()
{
    if (!Actions.empty() && CurrentActionIndex < Actions.size() && Actions[CurrentActionIndex])
        Actions[CurrentActionIndex]->Abort();
}

void Pattern::Reset()
{
    CurrentActionIndex = 0;
    bStarted = false;
}

// ----------------- ActionPatternSystem -----------------

void ActionPatternSystem::AddPattern(const eastl::string& Name, eastl::shared_ptr<Pattern> NewPattern)
{
    if (!NewPattern)
    {
        std::cerr << "[Warning] " << "\n";
        return;
    }

    if (Patterns.find(Name) != Patterns.end())
    {
        std::cerr << "[Warning] " << "\n";
        return;
    }

    Patterns[Name] = NewPattern;
}

void ActionPatternSystem::RemovePattern(const eastl::string& Name)
{
    auto it = Patterns.find(Name);

    if (it != Patterns.end())
        Patterns.erase(it);
}

EStateResult ActionPatternSystem::Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime)
{
    if (Patterns.empty())
        return EStateResult::Finished;

    if (!CurrentPattern)
    {
        float BestUtility = -eastl::numeric_limits<float>::infinity();

        for (auto& P : Patterns)
        {
            float Utility = P.second->EvaluateUtility(MBoard);

            if (Utility >= BestUtility)
            {
                BestUtility = Utility;
                CurrentPattern = P.second;
            }
        }

        CurrentPattern->OnPatternStart(GOID, MBoard);
    }

    auto Result = CurrentPattern->Update(GOID, MBoard, DeltaTime);

    if (Result != EActionCondition::Running)
    {
        CurrentPattern = nullptr;
    }
    else
    {
        return EStateResult::Running;
    }


    if (Result == EActionCondition::Aborted)
    {
        return EStateResult::Aborted;
    }
    else 
    {
        return EStateResult::Finished;
    }
}