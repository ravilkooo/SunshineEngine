#pragma once

#include "AI/Behavior/MemoryBoard.h"


class ActionPatternSystem
{
public:
	void SetMemory(MemoryBoard* NewMBoard) { MBoard = NewMBoard; }

private:
	MemoryBoard* MBoard = nullptr;
};

