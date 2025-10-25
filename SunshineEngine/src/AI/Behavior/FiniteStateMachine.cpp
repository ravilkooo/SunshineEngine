#include "AI/Behavior/FiniteStateMachine.h"

#include <iostream>


//FiniteStateMachine fsm;
//EventTransition toCombat("Combat", [&fsm](const eastl::string& state) {
//    fsm.ChangeState(state); });

void FiniteStateMachine::SetInitialState(const eastl::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		std::cerr << "[Warning]\n";
		return;
	}

	CurrentStateName = Name;
}

void FiniteStateMachine::RemoveState(eastl::string Name)
{
	if (CurrentStateName == Name)
	{
		CurrentStateName.clear();
	}

	States.erase(Name);
}

void FiniteStateMachine::Update(float DeltaTime)
{

}

void FiniteStateMachine::ChangeState(const eastl::string& newState)
{

}

bool ConditionTransition::ConditionTransitionCheck(MemoryBoard* MBoard, float DeltaTime)
{
	if (ConditionCheck(MBoard, DeltaTime))
	{
		Callback(ToState);

		return true;
	}

	return false;
}

bool EventTransition::Trigger(MemoryBoard* MBoard)
{
	if (TriggerCheck(MBoard))
	{
		Callback(ToState);

		return true;
	}

	return false;
}
