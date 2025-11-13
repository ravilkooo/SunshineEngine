#include "AI/Behavior/BehaviorController.h"


void BehaviorController::Update(float DeltaTime)
{
    if (!IsEnabled)
        return;

    FSM->Update(GOID, MBoard, DeltaTime);
}
