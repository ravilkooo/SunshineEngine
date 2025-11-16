#include "AI/Behavior/BehaviorController.h"

#include <iostream>


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Action
// ------------------------------------------------------------------------------------------------------

EActionCondition Action::Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
{
	if (IsAborted == true)
	{
		if (OnActionAbort)
		{
			OnActionAbort(GOID, MBoard);
		}

		IsAborted = false;

		return EActionCondition::Aborted;
	}

	EActionResult Result = EActionResult::Succeeded;

	if (OnActionUpdate)
	{
		Result = OnActionUpdate(GOID, MBoard, DeltaTime);
	}

	switch (Result)
	{
	case EActionResult::Succeeded:

		if (OnActionComplete)
		{
			OnActionComplete(GOID, MBoard, EActionResult::Succeeded);
		}

		return EActionCondition::Succeeded;

	case EActionResult::Failed:

		if (OnActionComplete)
		{
			OnActionComplete(GOID, MBoard, EActionResult::Failed);
		}

		return EActionCondition::Failed;

	case EActionResult::Running:

		return EActionCondition::Running;
	}
}


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Pattern
// ------------------------------------------------------------------------------------------------------

void Pattern::AddAction(eastl::shared_ptr<Action> NewAction)
{
	if (NewAction)
	{
		Actions.push_back(NewAction);
	}
	else
	{
		std::cerr << "[Warning] Pattern::AddAction: NewAction == nullptr\n";
	}
}

void Pattern::InsertAction(eastl::shared_ptr<Action> NewAction, size_t Index)
{
	if (!NewAction)
	{
		std::cerr << "[Warning] Pattern::InsertAction: NewAction == nullptr\n";
		return;
	}

	if (Index >= Actions.size())
	{
		Actions.push_back(NewAction);
	}
	else
	{
		Actions.insert(Actions.begin() + Index, NewAction);
	}

}

eastl::shared_ptr<Action> Pattern::GetActionByName(const eastl::string& Name) const
{
	for (auto& A : Actions)
	{
		if (A && A->Name == Name)
		{
			return A;
		}
	}

	std::cerr << "[Warning] Pattern::GetActionByName: action not found: " << Name.c_str() << "\n";
	return nullptr;
}

eastl::shared_ptr<Action> Pattern::GetActionByIndex(size_t Index) const
{
	if (Index >= Actions.size())
	{
		std::cerr << "[Warning] Pattern::GetActionByIndex: index out of range: " << Index << "\n";
		return nullptr;
	}

	return Actions[Index];
}

bool Pattern::RemoveActionByName(const eastl::string& Name)
{
	for (size_t i = 0; i < Actions.size(); ++i)
	{
		if (Actions[i] && Actions[i]->Name == Name)
		{
			Actions.erase(Actions.begin() + i);
			return true;
		}
	}

	std::cerr << "[Warning] Pattern::RemoveActionByName: action not found: " << Name.c_str() << "\n";
	return false;
}

bool Pattern::RemoveActionByIndex(size_t Index)
{
	if (Index >= Actions.size())
	{
		std::cerr << "[Warning] Pattern::RemoveActionByIndex: index out of range: " << Index << "\n";
		return false;
	}

	Actions.erase(Actions.begin() + Index);

	return true;
}

EActionCondition Pattern::Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
{
	if (OnPatternUpdate)
	{
		OnPatternUpdate(GOID, MBoard, DeltaTime);
	}

	if (Actions.empty())
		return EActionCondition::Succeeded;

	if (!bStarted)
	{
		if (Actions[CurrentActionIndex]->OnActionStart)
		{
			Actions[CurrentActionIndex]->OnActionStart(GOID, MBoard);
		}

		bStarted = true;
	}

	switch (Actions[CurrentActionIndex]->Update(GOID, MBoard, DeltaTime))
	{
	case EActionCondition::Succeeded:

		++CurrentActionIndex;

		if (CurrentActionIndex >= Actions.size())
		{
			Reset();

			if (OnPatternComplete)
			{
				OnPatternComplete(GOID, MBoard, EActionResult::Succeeded);
			}

			return EActionCondition::Succeeded;
		}
		else
		{
			bStarted = false;

			return EActionCondition::Running;
		}

	case EActionCondition::Failed:

		Reset();

		if (OnPatternComplete)
		{
			OnPatternComplete(GOID, MBoard, EActionResult::Failed);
		}

		return EActionCondition::Failed;

	case EActionCondition::Running:

		return EActionCondition::Running;

	case EActionCondition::Aborted:

		Reset();

		if (OnPatternAbort)
		{
			OnPatternAbort(GOID, MBoard);
		}

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


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- EventTransition
// ------------------------------------------------------------------------------------------------------

EventTransition::EventTransition(const eastl::string& InToState, CheckFunc InCheck, BehaviorController* FSM) : ToState(InToState), Check(InCheck)
{
	Abort = [FSM](const eastl::string& ToState)
			{
				if (FSM)
				{
					FSM->Abort(ToState);
				}
			};
}

void EventTransition::Trigger(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
{
	if (Check)
	{
		if (Check(GOID, MBoard))
		{
			Abort(ToState);
		}
	}
	else
	{
		Abort(ToState);
	}
}

void EventTransition::ChangeToState(const eastl::string& InToState, BehaviorController* FSM)
{
	ToState = InToState;

	Abort = [FSM](const eastl::string& ToState)
		{
			if (FSM)
			{
				FSM->Abort(ToState);
			}
		};
}


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- State
// ------------------------------------------------------------------------------------------------------

bool State::AddPattern(const eastl::string& Name, eastl::shared_ptr<Pattern> NewPattern)
{
	if (!NewPattern)
	{
		std::cerr << "[Warning] State::AddPattern: NewPattern == nullptr (name: " << Name.c_str() << ")\n";
		return false;
	}

	if (Patterns.find(Name) != Patterns.end())
	{
		std::cerr << "[Warning] State::AddPattern: pattern already exists: " << Name.c_str() << "\n";
		return false;
	}

	Patterns[Name] = NewPattern;

	return true;
}

eastl::shared_ptr<Pattern> State::GetPattern(const eastl::string& Name)
{
	auto it = Patterns.find(Name);

	if (it == Patterns.end())
	{
		std::cerr << "[Warning] State::GetPattern: pattern not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool State::RemovePattern(const eastl::string& Name)
{
	auto it = Patterns.find(Name);

	if (it == Patterns.end())
	{
		std::cerr << "[Warning] State::RemovePattern: pattern not found: " << Name.c_str() << "\n";
		return false;
	}

	Patterns.erase(it);

	return true;
}

void State::AddConditionTransition(const eastl::string& InToState, CheckFunc InCheck)
{
	auto Transition = eastl::make_shared<ConditionTransition>(InToState, InCheck);

	ConditionTransitions.push_back(Transition);
}

void State::AddEventTransition(const eastl::string& InToState, CheckFunc InCheck, BehaviorController* FSM)
{
	auto Transition = eastl::make_shared<EventTransition>(InToState, InCheck, FSM);

	EventTransitions.push_back(Transition);
}

bool State::RemoveConditionTransition(const eastl::string& ToState)
{
	for (auto it = ConditionTransitions.begin(); it != ConditionTransitions.end(); ++it)
	{
		if ((*it)->ToState == ToState)
		{
			ConditionTransitions.erase(it);
			return true;
		}
	}

	return false;
}

bool State::RemoveEventTransition(const eastl::string& ToState)
{
	for (auto it = EventTransitions.begin(); it != EventTransitions.end(); ++it)
	{
		if ((*it)->ToState == ToState)
		{
			EventTransitions.erase(it);
			return true;
		}
	}

	return false;
}

bool State::Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
{
	if (OnStateUpdate)
	{
		OnStateUpdate(GOID, MBoard, DeltaTime);
	}

	if (!CurrentPattern)
	{
		if (Patterns.empty())
		{
			if (OnStateExit)
			{
				OnStateExit(GOID, MBoard);
			}

			IsRunning = false;
			return false;
		}

		float BestUtility = -eastl::numeric_limits<float>::infinity();
		eastl::shared_ptr<Pattern> BestPattern = nullptr;

		for (auto& P : Patterns)
		{
			float Utility = -eastl::numeric_limits<float>::infinity();

			if (P.second->EvaluateUtility)
				Utility = P.second->EvaluateUtility(GOID, MBoard);

			if (Utility >= BestUtility)
			{
				BestUtility = Utility;
				BestPattern = P.second;
			}
		}

		CurrentPattern = BestPattern;

		if (CurrentPattern && CurrentPattern->OnPatternStart)
		{
			CurrentPattern->OnPatternStart(GOID, MBoard);
		}	
	}

	EActionCondition Result = CurrentPattern->Update(GOID, MBoard, DeltaTime);

	if (Result == EActionCondition::Running)
	{
		return false; 
	}

	if (Result == EActionCondition::Aborted)
	{
		if (OnStateAbort)
		{
			OnStateAbort(GOID, MBoard);
		}

		IsRunning = false;
		return true; 
	}
	else
	{
		if (OnStateExit)
		{
			OnStateExit(GOID, MBoard);
		}

		IsRunning = false;
		return false; 
	}
}

eastl::shared_ptr<ConditionTransition> State::CheckConditionTransitions(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
{
	for (auto& CT : ConditionTransitions)
	{
		if (CT->Check)
		{
			if (CT->Check(GOID, MBoard))
			{
				return CT;
			}
		}
	}

	return nullptr;
}


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- BehaviorController
// ------------------------------------------------------------------------------------------------------

void BehaviorController::SetMemoryBoard(const eastl::shared_ptr<MemoryBoard>& InMemoryBoard)
{
	if (!InMemoryBoard)
	{
		MBoard = nullptr;
		return;
	}

	MBoard = eastl::make_shared<MemoryBoard>(*InMemoryBoard);
}

bool BehaviorController::AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState)
{
	if (!NewState)
	{
		std::cerr << "[Warning] BehaviorController::AddState: NewState == nullptr (name: " << Name.c_str() << ")\n";
		return false;
	}

	if (States.find(Name) != States.end())
	{
		std::cerr << "[Warning] BehaviorController::AddState: state already exists: " << Name.c_str() << "\n";
		return false;
	}

	States[Name] = NewState;

	return true;
}

bool BehaviorController::RemoveState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		std::cerr << "[Warning] BehaviorController::RemoveState: state not found: " << Name.c_str() << "\n";
		return false;
	}

	States.erase(it);

	if (CurrentStateName == Name)
	{
		CurrentState.reset();
		CurrentStateName.clear();
	}

	return true;
}

eastl::shared_ptr<State> BehaviorController::GetState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		std::cerr << "[Warning] BehaviorController::GetState: state not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool BehaviorController::SetInitialState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		std::cerr << "[Warning] BehaviorController::SetInitialState: state not found: " << Name.c_str() << "\n";
		return false;
	}

	CurrentStateName = Name;
	CurrentState = it->second;

	return true;
}

bool BehaviorController::AddConditionTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck)
{
	auto From = GetState(FromState);
	auto To = GetState(ToState);

	if (!From || !To)
	{
		return false;
	}

	for (auto& CT : From->ConditionTransitions)
	{
		if (CT->ToState == ToState)
		{
			std::cerr << "[Warning] BehaviorController::AddConditionTransition: transition already exists: " << FromState.c_str() << " -> " << ToState.c_str() << "\n";
			return false;
		}
	}

	From->AddConditionTransition(ToState, InCheck);

	return true;
}

bool BehaviorController::AddEventTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck)
{
	auto From = GetState(FromState);
	auto To = GetState(ToState);

	if (!From || !To)
	{
		return false;
	}

	for (auto& ET : From->EventTransitions)
	{
		if (ET->ToState == ToState)
		{
			std::cerr << "[Warning] BehaviorController::AddEventTransition: transition already exists: " << FromState.c_str() << " -> " << ToState.c_str() << "\n";
			return false;
		}
	}

	From->AddEventTransition(ToState, InCheck, this);

	return true;
}

bool BehaviorController::ChangeToStateInConditionTransition(const eastl::string& FromState, const eastl::string& OldToState, const eastl::string& NewToState)
{
	auto From = GetState(FromState);

	if (!From || !GetState(NewToState))
	{
		return false;
	}

	for (auto& T : From->ConditionTransitions)
	{
		if (T->ToState == OldToState)
		{
			T->ToState = NewToState;
			return true;
		}
	}

	std::cerr << "[Warning]";
	return false; 
}

bool BehaviorController::ChangeToStateInEventTransition(const eastl::string& FromState, const eastl::string& OldToState, const eastl::string& NewToState)
{
	auto From = GetState(FromState);

	if (!From || !GetState(NewToState))
	{
		return false;
	}

	for (auto& T : From->EventTransitions)
	{
		if (T->ToState == OldToState)
		{
			T->ToState = NewToState;
			return true;
		}
	}

	std::cerr << "[Warning]";
	return false;
}

bool BehaviorController::ChangeCheckFuncInConditionTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck)
{
	auto From = GetState(FromState);

	if (!From)
	{
		return false;
	}

	for (auto& T : From->ConditionTransitions)
	{
		if (T->ToState == ToState)
		{
			T->Check = InCheck;
			return true;
		}
	}

	std::cerr << "[Warning]";
	return false;
}

bool BehaviorController::ChangeCheckFuncInEventTransition(const eastl::string& FromState, const eastl::string& ToState, CheckFunc InCheck)
{
	auto From = GetState(FromState);

	if (!From)
	{
		return false;
	}

	for (auto& T : From->EventTransitions)
	{
		if (T->ToState == ToState)
		{
			T->Check = InCheck;
			return true;
		}
	}

	std::cerr << "[Warning]";
	return false;
}

bool BehaviorController::RemoveConditionTransition(const eastl::string& FromState, const eastl::string& ToState)
{
	auto From = GetState(FromState);

	if (!From)
	{
		return false;
	}

	if (From->RemoveConditionTransition(ToState))
	{
		return true;
	}
	else
	{
		std::cerr << "[Warning] BehaviorController::RemoveConditionTransition: tried to remove non-existing transition from " << FromState.c_str() << " to " << ToState.c_str() << "\n";
		return false;
	}
}

bool BehaviorController::RemoveEventTransition(const eastl::string& FromState, const eastl::string& ToState)
{
	auto From = GetState(FromState);

	if (!From)
	{
		return false;
	}

	if (From->RemoveEventTransition(ToState))
	{
		return true;
	}
	else
	{
		std::cerr << "[Warning] BehaviorController::RemoveEventTransition: tried to remove non-existing transition from " << FromState.c_str() << " to " << ToState.c_str() << "\n";
		return false;
	}
}

void BehaviorController::Update(float DeltaTime)
{
	if (!IsEnabled)
	{
		return;
	}

	if (!CurrentState)
	{
		std::cerr << "[Warning] BehaviorController::Update: current state is null\n";
		return;
	}

	if (!CurrentState->IsRunning)
	{
		if (eastl::shared_ptr<ConditionTransition> CT = CurrentState->CheckConditionTransitions(GOID, MBoard))
		{
			ChangeState(GOID, MBoard, CT->ToState);
			return;
		}

		CurrentState->IsRunning = true;

		if (CurrentState->OnStateEnter)
		{
			CurrentState->OnStateEnter(GOID, MBoard);
		}
	}

	if (CurrentState->Update(GOID, MBoard, DeltaTime))
	{
		ChangeState(GOID, MBoard, AfterAbortStateName);
	}
}

void BehaviorController::Abort(const eastl::string& ToState)
{
	AfterAbortStateName = ToState;

	if (CurrentState && CurrentState->CurrentPattern)
		CurrentState->CurrentPattern->AbortCurrentAction();
}

void BehaviorController::ChangeState(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const eastl::string& NewState)
{
	if (NewState.empty())
	{
		std::cerr << "[Warning] ChangeState called with empty NewState\n";
		return;
	}

	if (NewState != CurrentStateName)
	{
		auto it = States.find(NewState);

		if (it == States.end())
		{
			std::cerr << "[Warning] BehaviorController::ChangeState: target state not found: " << NewState.c_str() << "\n";
			return;
		}

		CurrentStateName = NewState;
		CurrentState = it->second;
	}
}
