#pragma once
#include <EASTL/string.h>

static eastl::string sunshineErrorMessage;

void printSunshineErrorMessage();
void printSunshineErrorMessage(eastl::string errorMessage);
void printSunshineMessage(eastl::string message);
void printSunshineMessage(eastl::wstring message);
void printSunshineMessage(const char* format, eastl::string message);
