#include "AI/Behavior/BehaviorController.h"


void BehaviorController::Initialize()
{
    FSM = eastl::make_unique<FiniteStateMachine>();
    APS = eastl::make_unique<ActionPatternSystem>();

    FSM->SetMemory(&MBoard);
    APS->SetMemory(&MBoard);
}

void BehaviorController::Tick(float DeltaTime)
{
    if (!IsEnabled)
        return;
}

void BehaviorController::Reset()
{
    MBoard.Clear();
}