#include "AI/Behavior/BehaviorController.h"

// C++
#include <iostream>

// Lua
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Action
// ------------------------------------------------------------------------------------------------------

void Action::Lua_SetOnStart(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnActionStart = nullptr;
		return;
	}

	OnActionStart =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void Action::Lua_SetOnUpdate(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnActionUpdate = nullptr;
		return;
	}

	OnActionUpdate =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime) -> EActionResult
		{
			return static_cast<EActionResult>(Func(GOID, MBoard, DeltaTime));
		};
}

void Action::Lua_SetOnAbort(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnActionAbort = nullptr;
		return;
	}

	OnActionAbort =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void Action::Lua_SetOnComplete(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnActionComplete = nullptr;
		return;
	}

	OnActionComplete =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, EActionResult Result)
		{
			Func(GOID, MBoard, Result);
		};
}

EActionCondition Action::Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
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

eastl::shared_ptr<Action> Pattern::GetActionByName(const std::string& Name) const
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

bool Pattern::RemoveActionByName(const std::string& Name)
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

void Pattern::Lua_SetEvaluateUtility(const sol::function& Func)
{
	if (!Func.valid())
	{
		EvaluateUtility = nullptr;
		return;
	}

	EvaluateUtility =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard) -> float
		{
			return Func(GOID, MBoard);
		};
}

void Pattern::Lua_SetOnStart(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnPatternStart = nullptr;
		return;
	}

	OnPatternStart =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void Pattern::Lua_SetOnUpdate(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnPatternUpdate = nullptr;
		return;
	}

	OnPatternUpdate =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
		{
			Func(GOID, MBoard, DeltaTime);
		};
}

void Pattern::Lua_SetOnAbort(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnPatternAbort = nullptr;
		return;
	}

	OnPatternAbort =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void Pattern::Lua_SetOnComplete(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnPatternComplete = nullptr;
		return;
	}

	OnPatternComplete =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, EActionResult Result)
		{
			Func(GOID, MBoard, Result);
		};
}

EActionCondition Pattern::Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
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
	{
		Actions[CurrentActionIndex]->Abort();
	}
}

void Pattern::Reset()
{
	CurrentActionIndex = 0;
	bStarted = false;
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- EventTransition
// ------------------------------------------------------------------------------------------------------

EventTransition::EventTransition(const std::string& InToState, CheckFunc InCheck, BehaviorController* FSM) : ToState(InToState), Check(InCheck)
{
	Abort =
		[FSM](const std::string& ToState)
		{
			if (FSM)
			{
				FSM->Abort(ToState);
			}
		};
}

void EventTransition::Trigger(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
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

void EventTransition::ChangeToState(const std::string& InToState, BehaviorController* FSM)
{
	ToState = InToState;

	Abort = [FSM](const std::string& ToState)
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

bool State::AddPattern(const std::string& Name, eastl::shared_ptr<Pattern> NewPattern)
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

eastl::shared_ptr<Pattern> State::GetPattern(const std::string& Name)
{
	auto it = Patterns.find(Name);

	if (it == Patterns.end())
	{
		std::cerr << "[Warning] State::GetPattern: pattern not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool State::RemovePattern(const std::string& Name)
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

sol::table State::Lua_GetAllPatterns(sol::this_state ts) const
{
	sol::state_view lua(ts);
	sol::table t = lua.create_table();

	for (auto& [name, pattern] : Patterns)
	{
		t[name.c_str()] = pattern;
	}

	return t;
}

void State::Lua_SetOnEnter(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnStateEnter = nullptr;
		return;
	}

	OnStateEnter =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void State::Lua_SetOnUpdate(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnStateUpdate = nullptr;
		return;
	}

	OnStateUpdate =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
		{
			Func(GOID, MBoard, DeltaTime);
		};
}

void State::Lua_SetOnAbort(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnStateAbort = nullptr;
		return;
	}

	OnStateAbort =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void State::Lua_SetOnExit(const sol::function& Func)
{
	if (!Func.valid())
	{
		OnStateExit = nullptr;
		return;
	}

	OnStateExit =
		[Func](const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
		{
			Func(GOID, MBoard);
		};
}

void State::AddConditionTransition(const std::string& InToState, CheckFunc InCheck)
{
	auto Transition = eastl::make_shared<ConditionTransition>(InToState, InCheck);

	ConditionTransitions.push_back(Transition);
}

void State::AddEventTransition(const std::string& InToState, CheckFunc InCheck, BehaviorController* FSM)
{
	auto Transition = eastl::make_shared<EventTransition>(InToState, InCheck, FSM);

	EventTransitions.push_back(Transition);
}

bool State::RemoveConditionTransition(const std::string& ToState)
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

bool State::RemoveEventTransition(const std::string& ToState)
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

bool State::Update(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, float DeltaTime)
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

eastl::shared_ptr<ConditionTransition> State::CheckConditionTransitions(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard)
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

bool BehaviorController::AddState(const std::string& Name, const eastl::shared_ptr<State>& NewState)
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

bool BehaviorController::RemoveState(const std::string& Name)
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

eastl::shared_ptr<State> BehaviorController::GetState(const std::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		std::cerr << "[Warning] BehaviorController::GetState: state not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool BehaviorController::SetInitialState(const std::string& Name)
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

bool BehaviorController::AddConditionTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck)
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

bool BehaviorController::AddEventTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck)
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

bool BehaviorController::ChangeToStateInConditionTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState)
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

	std::cerr << "[Warning] BehaviorController::ChangeToStateInConditionTransition: No condition transition found in state " << FromState.c_str() << " from ToState " << OldToState.c_str() << "\n";
	return false; 
}

bool BehaviorController::ChangeToStateInEventTransition(const std::string& FromState, const std::string& OldToState, const std::string& NewToState)
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

	std::cerr << "[Warning] BehaviorController::ChangeToStateInEventTransition: No condition transition found in state " << FromState.c_str() << " from ToState " << OldToState.c_str() << "\n";
	return false;
}

bool BehaviorController::ChangeCheckFuncInConditionTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck)
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

bool BehaviorController::ChangeCheckFuncInEventTransition(const std::string& FromState, const std::string& ToState, CheckFunc InCheck)
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

bool BehaviorController::RemoveConditionTransition(const std::string& FromState, const std::string& ToState)
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

bool BehaviorController::RemoveEventTransition(const std::string& FromState, const std::string& ToState)
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

sol::table BehaviorController::Lua_GetAllStates(sol::this_state ts)
{
	sol::state_view lua(ts);
	sol::table t = lua.create_table();

	for (auto& [name, state] : States)
	{
		t[name.c_str()] = state;
	}

	return t;
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

void BehaviorController::Abort(const std::string& ToState)
{
	AfterAbortStateName = ToState;

	if (CurrentState && CurrentState->CurrentPattern)
		CurrentState->CurrentPattern->AbortCurrentAction();
}

void BehaviorController::ChangeState(const SE::UUID& GOID, const eastl::shared_ptr<MemoryBoard>& MBoard, const std::string& NewState)
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



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA BINDING
// ------------------------------------------------------------------------------------------------------

#define ACTION_ADD_FIELD(name) #name, &Action::name
#define ACTION_FIELD_PAIRS

#define ACTION_ADD_METHOD(k, fn) k, fn
#define ACTION_METHOD_PAIRS ACTION_LUA_METHODS_APPLY(ACTION_ADD_METHOD)

LUA_REGISTER_TYPE(Action, "Action", ACTION_FIELD_PAIRS, ACTION_METHOD_PAIRS)

#undef ACTION_ADD_METHOD
#undef ACTION_FIELD_PAIRS



#define PATTERN_ADD_FIELD(name) #name, &Pattern::name
#define PATTERN_FIELD_PAIRS

#define PATTERN_ADD_METHOD(k, fn) k, fn
#define PATTERN_METHOD_PAIRS PATTERN_LUA_METHODS_APPLY(PATTERN_ADD_METHOD)

LUA_REGISTER_TYPE(Pattern, "Pattern", PATTERN_FIELD_PAIRS, PATTERN_METHOD_PAIRS)

#undef PATTERN_ADD_METHOD
#undef PATTERN_FIELD_PAIRS



#define STATE_ADD_METHOD(k, fn) k, fn
#define STATE_METHOD_PAIRS STATE_LUA_METHODS_APPLY(STATE_ADD_METHOD)

#define STATE_FIELD_PAIRS
LUA_REGISTER_TYPE(State, "State", STATE_FIELD_PAIRS, STATE_METHOD_PAIRS)

#undef STATE_ADD_METHOD
#undef STATE_FIELD_PAIRS


/*
#define ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
	BehaviorController,
	"BehaviorController",
	//,
	BEHAVIORCONTROLLER_LUA_METHODS_APPLY(ADD_METHOD),
	"getBehaviorController"
)


#define BC_ADD_FIELD(name) #name, &BehaviorController::name
#define BEHAVIORCONTROLLER_FIELD_PAIRS \
    BEHAVIORCONTROLLER_LUA_FIELDS_APPLY(BC_ADD_FIELD)
#undef BC_ADD_FIELD

#define BC_ADD_METHOD_WITH_LEAD(k, fn) , k, fn
#define BEHAVIORCONTROLLER_METHOD_PAIRS \
    BEHAVIORCONTROLLER_LUA_METHODS_APPLY(BC_ADD_METHOD_WITH_LEAD)
#undef BC_ADD_METHOD_WITH_LEAD


LUA_REGISTER_COMPONENT(
	BehaviorController,
	"BehaviorController",
	BEHAVIORCONTROLLER_FIELD_PAIRS
	BEHAVIORCONTROLLER_METHOD_PAIRS,
	"getBehaviorController"
)
*/
