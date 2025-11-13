#include "AI/Behavior/FiniteStateMachine.h"

#include <iostream>


// ----------------- EventTransition -----------------

EventTransition::EventTransition(const eastl::string& InToState, FiniteStateMachine* FSM)
{
	Abort = [FSM](const eastl::string& ToState)
		{
			if (FSM)
				FSM->Abort(ToState);
		};
}

void EventTransition::Trigger(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
{
	if (Check)
	{
		if (Check(GOID, MBoard) && Abort)
		{
			Abort(ToState);
		}
	}
}


// ----------------- State -----------------

//eastl::shared_ptr<ConditionTransition> State::AddConditionTransition(const eastl::shared_ptr<ConditionTransition>& Transition)
//{
//	for (auto& CT : ConditionTransitions)
//	{
//		if (CT->ToState == Transition->ToState)
//		{
//			std::cerr << "[Warning] " << "\n";
//			return;
//		}
//	}
//
//	ConditionTransitions.push_back(Transition);
//}

eastl::shared_ptr<EventTransition> State::AddEventTransition(const eastl::string& InToState, FiniteStateMachine* FSM)
{
	for (auto& ET : EventTransitions)
	{
		if (ET->ToState == InToState)
		{
			std::cerr << "[Warning] " << "\n";
			return nullptr;
		}
	}

	auto Transition = eastl::make_shared<EventTransition>(InToState, FSM);

	EventTransitions.push_back(Transition);

	return Transition;
}

void State::RemoveConditionTransition(const eastl::string& ToState)
{
	for (auto it = ConditionTransitions.begin(); it != ConditionTransitions.end(); ++it)
	{
		if ((*it)->ToState == ToState)
		{
			ConditionTransitions.erase(it);
			return;
		}
	}

	std::cerr << "[Warning] Tried to remove non-existing ConditionTransition to: " << ToState.c_str() << "\n";
}

void State::RemoveEventTransition(const eastl::string& ToState)
{
	for (auto it = EventTransitions.begin(); it != EventTransitions.end(); ++it)
	{
		if ((*it)->ToState == ToState)
		{
			EventTransitions.erase(it);
			return;
		}
	}

	std::cerr << "[Warning] Tried to remove non-existing EventTransition to: " << ToState.c_str() << "\n";
}

bool State::Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
{
	if (OnStateUpdate)
	{
		OnStateUpdate(GOID, MBoard, DeltaTime);
	}

	switch (APS->Update(GOID, MBoard, DeltaTime))
	{
		case EStateResult::Running:

			return false;

		case EStateResult::Finished:

			if (OnStateExit)
			{
				OnStateExit(GOID, MBoard);
			}

			IsRunning = false;

			return false;

		case EStateResult::Aborted:

			if (OnStateAbort)
			{
				OnStateAbort(GOID, MBoard);
			}

			IsRunning = false;

			return true;
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


// ----------------- FiniteStateMachine -----------------

bool FiniteStateMachine::AddState(const eastl::string& Name, const eastl::shared_ptr<State>& NewState)
{
	if (!NewState)
	{
		std::cerr << "[Warning] " << "\n";
		return false;
	}

	if (States.find(Name) != States.end())
	{
		std::cerr << "[Warning] " << "\n";
		return false;
	}

	States[Name] = NewState;

	return true;
}

bool FiniteStateMachine::RemoveState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it != States.end())
	{
		States.erase(it);
	}
	else
	{
		return false;
	}

	if (CurrentStateName == Name)
	{
		CurrentState.reset();
		CurrentStateName.clear();
	}

	return true;
}

bool FiniteStateMachine::SetInitialState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it != States.end())
	{
		CurrentStateName = Name;
		CurrentState = it->second;

		return true;
	}

	std::cerr << "[Warning] " << "\n";
	return false;
}

void FiniteStateMachine::Update(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
{
	if (!CurrentState)
	{
		std::cerr << "[Warning] " << "\n";
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

void FiniteStateMachine::Abort(const eastl::string& ToState)
{
	AfterAbortStateName = ToState;

	if (CurrentState && CurrentState->APS->CurrentPattern)
		CurrentState->APS->CurrentPattern->AbortCurrentAction();
}

void FiniteStateMachine::ChangeState(const Sunshine::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const eastl::string& NewState)
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
			std::cerr << "[Warning] " << "\n";
			return;
		}

		CurrentStateName = NewState;
		CurrentState = it->second;
	}
}

eastl::shared_ptr<State> FiniteStateMachine::GetState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it != States.end())
	{
		return it->second;
	}

	std::cerr << "[Warning] " << "\n";

	return nullptr;
}
