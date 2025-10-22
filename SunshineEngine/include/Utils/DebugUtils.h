#pragma once
#include <EASTL/string.h>

eastl::string sunshineErrorMessage;

void printSunshineErrorMessage()
{
	printf("%s\n", sunshineErrorMessage.c_str());
}

void printSunshineMessage(eastl::string message)
{
	printf("%s\n", message.c_str());
}


