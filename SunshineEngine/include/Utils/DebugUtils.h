#pragma once
#include <EASTL/string.h>

static eastl::string sunshineErrorMessage;

void printSunshineErrorMessage();
void printSunshineMessage(eastl::string message);
void printSunshineMessage(const char* format, eastl::string message);
