#include "AI/Behavior/BehaviorController.h"


BehaviorController::~BehaviorController()
{
    MBoard.Clear();
}

void BehaviorController::Update(float DeltaTime)
{
    if (!IsEnabled)
        return;

    FSM->Update(GOID, &MBoard, DeltaTime);
}