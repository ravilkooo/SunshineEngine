#include "Utils/DebugUtils.h"

void printSunshineErrorMessage()
{
	printf("%s\n", sunshineErrorMessage.c_str());
}

void printSunshineErrorMessage(eastl::string errorMessage)
{
	printf("%s\n", errorMessage.c_str());
}

void printSunshineErrorMessage(eastl::wstring errorMessage)
{
	wprintf(L"%ls\n", errorMessage.c_str());
}

void printSunshineMessage(eastl::string message)
{
	printf("%s\n", message.c_str());
}

void printSunshineMessage(eastl::wstring message)
{
	wprintf(L"%ls\n", message.c_str());
}
