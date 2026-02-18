#include "Utils/DebugUtils.h"
#include <Utils/StringUtils.h>
#include "Utils/GameLogManager.h"

void printSunshineErrorMessage()
{
	//printf("%s\n", sunshineErrorMessage.c_str());
	LOG_GAME_ERROR("%s\n", sunshineErrorMessage.c_str());
}

void printSunshineErrorMessage(eastl::string errorMessage)
{
	LOG_GAME_ERROR("%s\n", errorMessage.c_str());
}

void printSunshineMessage(eastl::string message)
{
	LOG_GAME_INFO("%s\n", message.c_str());
}

void printSunshineMessage(eastl::wstring message)
{
	LOG_GAME_INFO("%s\n", WStringToUtf8(message).c_str());
}

void printSunshineErrorMessage(eastl::wstring errorMessage)
{
	LOG_GAME_ERROR("%s\n", WStringToUtf8(errorMessage).c_str());
}