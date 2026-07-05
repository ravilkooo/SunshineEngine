#include "AI/Behavior/BehaviorController.h"

// C++
#include <iostream>

// Lua
#include <Scripting/AutoBindings.h>
#include <Scripting/ComponentBindings.h>


// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Action
// ------------------------------------------------------------------------------------------------------

EActionCondition Action::Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime)
{
	if (IsAborted == true)
	{
		if (OnActionAbort)
		{
			OnActionAbort(GOID.GetHilo(), BC);
		}

		IsAborted = false;

		return EActionCondition::Aborted;
	}

	EActionResult Result = EActionResult::Succeeded;

	if (OnActionUpdate)
	{
		sol::protected_function_result CallResult = OnActionUpdate(GOID.GetHilo(), BC, DeltaTime);

		if (!CallResult.valid())
		{
			sol::error Err = CallResult;
			//std::cerr << "[Warning] OnActionUpdate error: " << Err.what() << "\n";
		}
		else if (CallResult.return_count() == 0)
		{
			//std::cerr << "[Warning] OnActionUpdate returned nothing\n";
		}
		else
		{
			sol::object Ret = CallResult.get<sol::object>();

			if (Ret.is<int>())
			{
				const int Value = Ret.as<int>();

				if (Value >= static_cast<int>(EActionResult::Running) && Value <= static_cast<int>(EActionResult::Failed))
				{
					Result = static_cast<EActionResult>(Value);
				}
				else
				{
					//std::cerr << "[Warning] OnActionUpdate returned invalid EActionResult value: " << Value << "\n";
				}
			}
			else
			{
				//std::cerr << "[Warning] OnActionUpdate returned non-integer value\n";
			}
		}
	}

	switch (Result)
	{
		case EActionResult::Succeeded:

			if (OnActionComplete)
			{
				OnActionComplete(GOID.GetHilo(), BC, 1);
			}

			return EActionCondition::Succeeded;

		case EActionResult::Failed:

			if (OnActionComplete)
			{
				OnActionComplete(GOID.GetHilo(), BC, 2);
			}

			return EActionCondition::Failed;

		case EActionResult::Running:

			return EActionCondition::Running;
	}
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- Pattern
// ------------------------------------------------------------------------------------------------------

// --- Actions ---
std::shared_ptr<Action> Pattern::Action_GetByName(const std::string& Name) const
{
	for (auto& A : Actions)
	{
		if (A && A->Name == Name)
		{
			return A;
		}
	}

	//std::cerr << "[Warning] Pattern::GetActionByName: action not found: " << Name.c_str() << "\n";
	return nullptr;
}

std::shared_ptr<Action> Pattern::Action_GetByIndex(size_t Index) const
{
	if (Index >= Actions.size())
	{
		//std::cerr << "[Warning] Pattern::GetActionByIndex: index out of range: " << Index << "\n";
		return nullptr;
	}

	return Actions[Index];
}

bool Pattern::Action_Add(const std::string& Name)
{
	for (const std::shared_ptr<Action>& Action : Actions)
	{
		if (Action && Action->GetName() == Name)
		{
			//std::cerr << "[Warning] State::Action_Add: action already exists: " << Name.c_str() << "\n";
			return false;
		}
	}

	Actions.push_back(std::make_shared<Action>(Name));

	return true;
}

bool Pattern::Action_Insert(const std::string& Name, size_t Index)
{
	for (const std::shared_ptr<Action>& Action : Actions)
	{
		if (Action && Action->GetName() == Name)
		{
			//std::cerr << "[Warning] Pattern::Action_Insert: action already exists: " << Name.c_str() << "\n";
			return false;
		}
	}

	auto NewAction = std::make_shared<Action>(Name);

	if (Index >= Actions.size())
	{
		Actions.push_back(NewAction);
	}
	else
	{
		Actions.insert(Actions.begin() + Index, NewAction);
	}

	return true;
}

sol::table Pattern::Action_GetAll(sol::this_state L)
{
	sol::state_view Lua(L);
	sol::table Result = Lua.create_table(static_cast<int>(Actions.size()), 0);

	int Index = 1;
	for (const std::shared_ptr<Action>& ActionPtr : Actions)
	{
		Result[Index++] = ActionPtr;
	}

	return Result;
}

bool Pattern::Action_RemoveByName(const std::string& Name)
{
	for (size_t i = 0; i < Actions.size(); ++i)
	{
		if (Actions[i] && Actions[i]->Name == Name)
		{
			Actions.erase(Actions.begin() + i);
			return true;
		}
	}

	//std::cerr << "[Warning] Pattern::RemoveActionByName: action not found: " << Name.c_str() << "\n";
	return false;
}

bool Pattern::Action_RemoveByIndex(size_t Index)
{
	if (Index >= Actions.size())
	{
		//std::cerr << "[Warning] Pattern::RemoveActionByIndex: index out of range: " << Index << "\n";
		return false;
	}

	Actions.erase(Actions.begin() + Index);

	return true;
}
//

// --- In game ---
EActionCondition Pattern::Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime)
{
	if (OnPatternUpdate)
	{
		OnPatternUpdate(GOID.GetHilo(), BC, DeltaTime);
	}

	if (Actions.empty())
		return EActionCondition::Succeeded;

	if (!bStarted)
	{
		if (Actions[CurrentActionIndex]->OnActionStart)
		{
			Actions[CurrentActionIndex]->OnActionStart(GOID.GetHilo(), BC);
		}

		bStarted = true;
	}

	switch (Actions[CurrentActionIndex]->Update(GOID, BC, DeltaTime))
	{
	case EActionCondition::Succeeded:

		++CurrentActionIndex;

		if (CurrentActionIndex >= Actions.size())
		{
			Reset();

			if (OnPatternComplete)
			{
				OnPatternComplete(GOID.GetHilo(), BC, 1);
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
			OnPatternComplete(GOID.GetHilo(), BC, 2);
		}

		return EActionCondition::Failed;

	case EActionCondition::Running:

		return EActionCondition::Running;

	case EActionCondition::Aborted:

		Reset();

		if (OnPatternAbort)
		{
			OnPatternAbort(GOID.GetHilo(), BC);
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
//



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- State
// ------------------------------------------------------------------------------------------------------

// --- Patterns ---
std::shared_ptr<Pattern> State::Pattern_Get(const std::string& Name)
{
	auto it = Patterns.find(Name);

	if (it == Patterns.end())
	{
		//std::cerr << "[Warning] State::GetPattern: pattern not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool State::Pattern_Add(const std::string& Name)
{
	if (Patterns.find(Name) != Patterns.end())
	{
		//std::cerr << "[Warning] State::AddPattern: pattern already exists: " << Name.c_str() << "\n";
		return false;
	}

	Patterns[Name] = std::make_shared<Pattern>();

	return true;
}

bool State::Pattern_Remove(const std::string& Name)
{
	auto it = Patterns.find(Name);

	if (it == Patterns.end())
	{
		//std::cerr << "[Warning] State::RemovePattern: pattern not found: " << Name.c_str() << "\n";
		return false;
	}

	Patterns.erase(it);

	return true;
}

sol::table State::Pattern_GetAll(sol::this_state L) const
{
	sol::state_view lua(L);
	sol::table t = lua.create_table();

	int index = 1;
	for (const auto& [name, pattern] : Patterns)
	{
		t[index++] = name;
	}

	return t;
}
//

// --- Transition Conditions ---
void State::ConditionTransition_Add(const std::string& InToState, sol::function InCheck)
{
	auto Transition = std::make_shared<ConditionTransition>(InToState, InCheck);

	ConditionTransitions.push_back(Transition);
}

bool State::ConditionTransition_Remove(const std::string& ToState)
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
//

// --- In game ---
bool State::Update(const SE::UUID& GOID, BehaviorController* BC, float DeltaTime)
{
	if (OnStateUpdate)
	{
		OnStateUpdate(GOID.GetHilo(), BC, DeltaTime);
	}

	if (!CurrentPattern)
	{
		if (Patterns.empty())
		{
			if (OnStateExit)
			{
				OnStateExit(GOID.GetHilo(), BC);
			}

			IsRunning = false;
			return false;
		}

		float BestUtility = -std::numeric_limits<float>::infinity();
		std::shared_ptr<Pattern> BestPattern = nullptr;

		for (auto& P : Patterns)
		{
			float Utility = -std::numeric_limits<float>::infinity();

			if (P.second->EvaluateUtility)
			{
				sol::protected_function_result res = P.second->EvaluateUtility(GOID.GetHilo(), BC);

				if (res.valid())
				{
					if (res.get_type() == sol::type::number)
					{
						Utility = res.get<float>();
					}
					else
					{
						//
					}
				}
			}

			if (Utility >= BestUtility)
			{
				BestUtility = Utility;
				BestPattern = P.second;
			}
		}

		CurrentPattern = BestPattern;

		if (CurrentPattern && CurrentPattern->OnPatternStart)
		{
			CurrentPattern->OnPatternStart(GOID.GetHilo(), BC);
		}	
	}

	EActionCondition Result = CurrentPattern->Update(GOID, BC, DeltaTime);

	if (Result == EActionCondition::Running)
	{
		return false; 
	}

	if (Result == EActionCondition::Aborted)
	{
		if (OnStateAbort)
		{
			OnStateAbort(GOID.GetHilo(), BC);
		}

		CurrentPattern = nullptr;

		IsRunning = false;
		return true; 
	}
	else
	{
		if (OnStateExit)
		{
			OnStateExit(GOID.GetHilo(), BC);
		}

		CurrentPattern = nullptr;

		IsRunning = false;
		return false; 
	}
}

std::shared_ptr<ConditionTransition> State::CheckConditionTransitions(const SE::UUID& GOID, BehaviorController* BC)
{
	for (auto& CT : ConditionTransitions)
	{
		if (CT->Check)
		{
			bool Result = false;

			sol::protected_function_result CallResult = CT->Check(GOID.GetHilo(), BC);

			if (!CallResult.valid())
			{
				sol::error Err = CallResult;
				//std::cerr << "[Warning] State::CheckConditionTransitions error: " << Err.what() << "\n";
			}
			else if (CallResult.return_count() == 0)
			{
				//std::cerr << "[Warning] State::CheckConditionTransitions returned nothing\n";
			}
			else
			{
				sol::object Ret = CallResult.get<sol::object>();

				if (Ret.is<bool>())
				{
					Result = Ret.as<bool>();
				}
				else
				{
					//std::cerr << "[Warning] State::CheckConditionTransitions returned non-bool value\n";
				}
			}

			if (Result)
			{
				return CT;
			}
		}
	}

	return nullptr;
}
//



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- BehaviorController
// ------------------------------------------------------------------------------------------------------

// --- General ---
sol::table BehaviorController::GetAllStates(sol::this_state L) const
{
	sol::state_view lua(L);
	sol::table t = lua.create_table();

	int index = 1;
	for (const auto& [name, state] : States)
	{
		t[index++] = name;
	}

	return t;
}

void BehaviorController::Trigger(const std::string& ToState)
{
	auto it = States.find(ToState);

	if (it == States.end())
	{
		return;
	}

	if (ToState != CurrentStateName)
		Abort(ToState);
}

void BehaviorController::FromJson(const json& j)
{
	if (j.contains("IsEnabled") && j["IsEnabled"].is_boolean())
	{
		IsEnabled = j.at("IsEnabled").get<bool>();
	}
}
//

// --- MemoryBoard ---
sol::object BehaviorController::MemoryBoard_GetInt(const std::string& Key, sol::this_state L) const
{
	int v;

	if (!MBoard->GetInt(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v);
}

sol::object BehaviorController::MemoryBoard_GetFloat(const std::string& Key, sol::this_state L) const
{
	float v;

	if (!MBoard->GetFloat(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v);
}

sol::object BehaviorController::MemoryBoard_GetBool(const std::string& Key, sol::this_state L) const
{
	bool v;

	if (!MBoard->GetBool(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v);
}

sol::object BehaviorController::MemoryBoard_GetString(const std::string& Key, sol::this_state L) const
{
	std::string v;

	if (!MBoard->GetString(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v);
}

sol::object BehaviorController::MemoryBoard_GetVector3(const std::string& Key, sol::this_state L) const
{
	DXSM::Vector3 v;

	if (!MBoard->GetVector3(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v);
}

sol::object BehaviorController::MemoryBoard_GetUUID(const std::string& Key, sol::this_state L) const
{
	SE::UUID v;

	if (!MBoard->GetUUID(Key, v))
	{
		return sol::nil;
	}

	return sol::make_object(L, v.GetHilo());
}
//

// --- States ---
std::shared_ptr<State> BehaviorController::State_Get(const std::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		//std::cerr << "[Warning] BehaviorController::GetState: state not found: " << Name.c_str() << "\n";
		return nullptr;
	}

	return it->second;
}

bool BehaviorController::State_Add(const std::string& Name)
{
	if (States.find(Name) != States.end())
	{
		//std::cerr << "[Warning] BehaviorController::AddState: state already exists: " << Name.c_str() << "\n";
		return false;
	}

	States[Name] = std::make_shared<State>();

	return true;
}

bool BehaviorController::State_Remove(const std::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		//std::cerr << "[Warning] BehaviorController::RemoveState: state not found: " << Name.c_str() << "\n";
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

bool BehaviorController::State_SetInitial(const std::string& Name)
{
	auto it = States.find(Name);

	if (it == States.end())
	{
		//std::cerr << "[Warning] BehaviorController::SetInitialState: state not found: " << Name.c_str() << "\n";
		return false;
	}

	CurrentStateName = Name;
	CurrentState = it->second;

	return true;
}

sol::table BehaviorController::State_GetAllPatterns(const std::string& Name, sol::this_state L)
{
	auto S = State_Get(Name);

	if (S)
	{
		return S->Pattern_GetAll(L);
	}

	return sol::table();
}

bool BehaviorController::State_SetOnEnter(const std::string& Name, const sol::function& Func)
{
	auto S = State_Get(Name);

	if (S)
	{
		S->OnStateEnter = Func;

		return true;
	}

	return false;
}

bool BehaviorController::State_SetOnUpdate(const std::string& Name, const sol::function& Func)
{
	auto S = State_Get(Name);

	if (S)
	{
		S->OnStateUpdate = Func;

		return true;
	}

	return false;
}

bool BehaviorController::State_SetOnAbort(const std::string& Name, const sol::function& Func)
{
	auto S = State_Get(Name);

	if (S)
	{
		S->OnStateAbort = Func;

		return true;
	}

	return false;
}

bool BehaviorController::State_SetOnExit(const std::string& Name, const sol::function& Func)
{
	auto S = State_Get(Name);

	if (S)
	{
		S->OnStateExit = Func;

		return true;
	}

	return false;
}
//

// --- Transition Conditions ---
bool BehaviorController::ConditionTransition_Add(const std::string& FromState, const std::string& ToState, sol::function InCheck)
{
	auto From = State_Get(FromState);
	auto To = State_Get(ToState);

	if (!From || !To)
	{
		return false;
	}

	for (auto& CT : From->ConditionTransitions)
	{
		if (CT->ToState == ToState)
		{
			//std::cerr << "[Warning] BehaviorController::AddConditionTransition: transition already exists: " << FromState.c_str() << " -> " << ToState.c_str() << "\n";
			return false;
		}
	}

	From->ConditionTransition_Add(ToState, InCheck);

	return true;
}

bool BehaviorController::ConditionTransition_Has(const std::string& FromState, const std::string& ToState)
{
	auto From = State_Get(FromState);
	auto To = State_Get(ToState);

	if (!From || !To)
	{
		return false;
	}

	for (auto& CT : From->ConditionTransitions)
	{
		if (CT->ToState == ToState)
		{
			return true;
		}
	}

	return false;
}

bool BehaviorController::ConditionTransition_ChangeToState(const std::string& FromState, const std::string& OldToState, const std::string& NewToState)
{
	auto From = State_Get(FromState);

	if (!From || !State_Get(NewToState))
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

	//std::cerr << "[Warning] BehaviorController::ChangeToStateInConditionTransition: No condition transition found in state " << FromState.c_str() << " from ToState " << OldToState.c_str() << "\n";
	return false; 
}

bool BehaviorController::ConditionTransition_ChangeCheckFunc(const std::string& FromState, const std::string& ToState, sol::function InCheck)
{
	auto From = State_Get(FromState);

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

	//std::cerr << "[Warning]";
	return false;
}

bool BehaviorController::ConditionTransition_Remove(const std::string& FromState, const std::string& ToState)
{
	auto From = State_Get(FromState);

	if (!From)
	{
		return false;
	}

	if (From->ConditionTransition_Remove(ToState))
	{
		return true;
	}
	else
	{
		//std::cerr << "[Warning] BehaviorController::RemoveConditionTransition: tried to remove non-existing transition from " << FromState.c_str() << " to " << ToState.c_str() << "\n";
		return false;
	}
}
//

// --- Patterns ---
bool BehaviorController::Pattern_Add(const std::string& NameS, const std::string& NameP)
{
	auto S = State_Get(NameS);

	if (S)
	{
		return S->Pattern_Add(NameP);
	}

	return false;
}

bool BehaviorController::Pattern_Remove(const std::string& NameS, const std::string& NameP)
{
	auto S = State_Get(NameS);

	if (S)
	{
		return S->Pattern_Remove(NameP);
	}

	return false;
}

sol::table BehaviorController::Pattern_GetAllActions(const std::string& NameS, const std::string& NameP, sol::this_state L)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Action_GetAll(L);
		}
	}

	return sol::table();
}

int BehaviorController::Pattern_Count(const std::string& NameS, const std::string& NameP)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Actions.size();
		}
	}
	
	return -1;
}

bool BehaviorController::Pattern_SetEvaluateUtility(const std::string& NameS, const std::string& NameP, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			P->EvaluateUtility = Func;

			return true;
		}
	}

	return false;
}

bool BehaviorController::Pattern_SetOnStart(const std::string& NameS, const std::string& NameP, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			P->OnPatternStart = Func;

			return true;
		}
	}

	return false;
}

bool BehaviorController::Pattern_SetOnUpdate(const std::string& NameS, const std::string& NameP, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			P->OnPatternUpdate = Func;

			return true;
		}
	}

	return false;
}

bool BehaviorController::Pattern_SetOnAbort(const std::string& NameS, const std::string& NameP, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			P->OnPatternAbort = Func;

			return true;
		}
	}

	return false;
}

bool BehaviorController::Pattern_SetOnComplete(const std::string& NameS, const std::string& NameP, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			P->OnPatternComplete = Func;

			return true;
		}
	}

	return false;
}
//

// --- Actions ---
bool BehaviorController::Action_Add(const std::string& NameS, const std::string& NameP, const std::string& NameA)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Action_Add(NameA);
		}
	}

	return false;
}

bool BehaviorController::Action_Insert(const std::string& NameS, const std::string& NameP, const std::string& NameA, size_t Index)
{
	if (Index < 0)
		return false;

	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Action_Insert(NameA, Index);
		}
	}

	return false;
}

bool BehaviorController::Action_RemoveByName(const std::string& NameS, const std::string& NameP, const std::string& NameA)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Action_RemoveByName(NameA);
		}
	}

	return false;
}

bool BehaviorController::Action_RemoveByIndex(const std::string& NameS, const std::string& NameP, size_t Index)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			return P->Action_RemoveByIndex(Index);
		}
	}

	return false;
}

bool BehaviorController::Action_SetOnStart(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			auto A = P->Action_GetByName(NameA);

			if (A)
			{
				A->OnActionStart = Func;

				return true;
			}
		}
	}

	return false;
}

bool BehaviorController::Action_SetOnUpdate(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			auto A = P->Action_GetByName(NameA);

			if (A)
			{
				A->OnActionUpdate = Func;

				return true;
			}
		}
	}

	return false;
}

bool BehaviorController::Action_SetOnAbort(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			auto A = P->Action_GetByName(NameA);

			if (A)
			{
				A->OnActionAbort = Func;

				return true;
			}
		}
	}

}

bool BehaviorController::Action_SetOnComplete(const std::string& NameS, const std::string& NameP, const std::string& NameA, const sol::function& Func)
{
	auto S = State_Get(NameS);

	if (S)
	{
		auto P = S->Pattern_Get(NameP);

		if (P)
		{
			auto A = P->Action_GetByName(NameA);

			if (A)
			{
				A->OnActionComplete = Func;

				return true;
			}
		}
	}

}
//

// --- In game ---
void BehaviorController::Update(float DeltaTime)
{
	if (!IsEnabled)
	{
		return;
	}

	if (!CurrentState)
	{
		//std::cerr << "[Warning] BehaviorController::Update: current state is null\n";
		return;
	}

	if (!CurrentState->IsRunning)
	{
		if (std::shared_ptr<ConditionTransition> CT = CurrentState->CheckConditionTransitions(GOID, this))
		{
			ChangeState(GOID, CT->ToState);
			return;
		}

		CurrentState->IsRunning = true;

		if (CurrentState->OnStateEnter)
		{
			CurrentState->OnStateEnter(GOID.GetHilo(), this);
		}
	}

	if (CurrentState->Update(GOID, this, DeltaTime))
	{
		ChangeState(GOID, AfterAbortStateName);
	}
}

void BehaviorController::Abort(const std::string& ToState)
{
	AfterAbortStateName = ToState;

	if (CurrentState && CurrentState->CurrentPattern)
		CurrentState->CurrentPattern->AbortCurrentAction();
}

void BehaviorController::ChangeState(const SE::UUID& GOID, const std::string& NewState)
{
	if (NewState.empty())
	{
		//std::cerr << "[Warning] ChangeState called with empty NewState\n";
		return;
	}

	if (NewState != CurrentStateName)
	{
		auto it = States.find(NewState);

		if (it == States.end())
		{
			//std::cerr << "[Warning] BehaviorController::ChangeState: target state not found: " << NewState.c_str() << "\n";
			return;
		}

		CurrentStateName = NewState;
		CurrentState = it->second;
	}
}
//



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- BehaviorController_Info
// ------------------------------------------------------------------------------------------------------

json BehaviorController_Info::ToJson() const
{
	json j;

	j = nlohmann::json
	{
		{"IsEnabled",           IsEnabled},
	};

	return j;
}

void BehaviorController_Info::FromJson(const json& j)
{
	if (j.contains("IsEnabled") && j["IsEnabled"].is_boolean())
	{
		IsEnabled = j.at("IsEnabled").get<bool>();
	}
}



// ------------------------------------------------------------------------------------------------------
// ---------------------------------- LUA BINDING
// ------------------------------------------------------------------------------------------------------

#define ADD_METHOD(k, fn) k, fn

LUA_REGISTER_COMPONENT(
	BehaviorController,
	"BehaviorController",
	/* no fields */,
	/* no properties */,
	BEHAVIORCONTROLLER_LUA_METHODS_APPLY(ADD_METHOD),
	"getBehavior"
)
