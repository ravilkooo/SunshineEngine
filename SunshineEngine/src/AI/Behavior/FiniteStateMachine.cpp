#include "AI/Behavior/FiniteStateMachine.h"

#include <iostream>


// ----------------- EventTransition -----------------

void EventTransition::Trigger(const Sunshine::UUID& GOID, MemoryBoard* MBoard)
{
	if (TriggerCheck(GOID, MBoard) && Callback)
	{
		Callback(ToState);
	}
}


// ----------------- State -----------------

void State::AddConditionTransition(const eastl::shared_ptr<ConditionTransition>& Transition)
{
	for (auto& CT : ConditionTransitions)
	{
		if (CT->GetTargetState() == Transition->GetTargetState())
		{
			std::cerr << "[Warning] " << "\n";
			return;
		}
	}

	ConditionTransitions.push_back(Transition);
}

void State::AddEventTransition(const eastl::shared_ptr<EventTransition>& Transition)
{
	for (auto& ET : EventTransitions)
	{
		if (ET->GetTargetState() == Transition->GetTargetState())
		{
			std::cerr << "[Warning] " << "\n";
			return;
		}
	}

	EventTransitions.push_back(Transition);
}

void State::RemoveConditionTransition(const eastl::string& TargetState)
{
	for (auto it = ConditionTransitions.begin(); it != ConditionTransitions.end(); ++it)
	{
		if ((*it)->GetTargetState() == TargetState)
		{
			ConditionTransitions.erase(it);
			return;
		}
	}

	std::cerr << "[Warning] Tried to remove non-existing ConditionTransition: " << TargetState.c_str() << "\n";
}

void State::RemoveEventTransition(const eastl::string& TargetState)
{
	for (auto it = EventTransitions.begin(); it != EventTransitions.end(); ++it)
	{
		if ((*it)->GetTargetState() == TargetState)
		{
			EventTransitions.erase(it);
			return;
		}
	}

	std::cerr << "[Warning] Tried to remove non-existing EventTransition: " << TargetState.c_str() << "\n";
}

eastl::shared_ptr<ConditionTransition> State::CheckConditionTransitions(MemoryBoard* MBoard, float DeltaTime)
{
	for (auto& CT : ConditionTransitions)
	{
		if (CT->ConditionTransitionCheck(MBoard, DeltaTime))
			return CT;
	}

	return nullptr;
}

bool State::Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime)
{
	OnStateUpdate(GOID, MBoard, DeltaTime);

	switch (APS->Update(GOID, MBoard, DeltaTime))
	{
		case EStateResult::Running:

			return false;

		case EStateResult::Finished:

			OnStateExit(GOID, MBoard);

			return false;

		case EStateResult::Aborted:

			OnStateAbort(GOID, MBoard);

			return true;
	}
}


// ----------------- FiniteStateMachine -----------------

void FiniteStateMachine::AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState)
{
	if (!NewState)
	{
		std::cerr << "[Warning] " << "\n";
		return;
	}

	if (States.find(Name) != States.end())
	{
		std::cerr << "[Warning] " << "\n";
		return;
	}

	States[Name] = NewState;
}

void FiniteStateMachine::RemoveState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it != States.end())
		States.erase(it);

	if (CurrentStateName == Name)
	{
		CurrentState.reset();
		CurrentStateName.clear();
	}
}

void FiniteStateMachine::SetInitialState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it != States.end())
	{
		CurrentStateName = Name;
		CurrentState = it->second;
	}
	else
	{
		std::cerr << "[Warning] " << "\n";
	}
}

void FiniteStateMachine::Update(const Sunshine::UUID& GOID, MemoryBoard* MBoard, float DeltaTime)
{
	if (!CurrentState)
	{
		std::cerr << "[Warning] " << "\n";
		return;
	}

	if (!CurrentState->IsRunning)
	{
		if (eastl::shared_ptr<ConditionTransition> CT = CurrentState->CheckConditionTransitions(MBoard, DeltaTime))
		{
			ChangeState(GOID, MBoard, CT->GetTargetState());
			return;
		}

		CurrentState->IsRunning = true;

		CurrentState->OnStateEnter(GOID, MBoard);
	}

	if (CurrentState->Update(GOID, MBoard, DeltaTime))
	{
		ChangeState(GOID, MBoard, AfterAbortStateName);
	}
}

void FiniteStateMachine::Abort(const eastl::string& ToState)
{
	AfterAbortStateName = ToState;

	if (CurrentState && CurrentState->APS->CurrentPattern)
		CurrentState->APS->CurrentPattern->AbortCurrentAction();
}

void FiniteStateMachine::ChangeState(const Sunshine::UUID& GOID, MemoryBoard* MBoard, const eastl::string& NewState)
{
	if (NewState.empty())
	{
		std::cerr << "[Warning] ChangeState called with empty NewState\n";
		return;
	}

	if (NewState == CurrentStateName)
		return;

	if (CurrentState)
	{
		CurrentState->IsRunning = false;

		if (AfterAbortStateName != "")
		{
			CurrentState->OnStateAbort(GOID, MBoard);

			AfterAbortStateName = "";
		}
		else
		{
			CurrentState->OnStateExit(GOID, MBoard);
		}
	}

	auto it = States.find(NewState);

	if (it == States.end())
	{
		std::cerr << "[Warning] " << "\n";
		return;
	}

	CurrentStateName = NewState;
	CurrentState = it->second;
}