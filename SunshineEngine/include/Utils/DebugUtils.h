#pragma once
#include <EASTL/string.h>

namespace sunshinedebug 
{
	eastl::string errorMessage;

	void printErrorMessage()
	{
		printf("%s\n", errorMessage.c_str());
	}

	void printMessage(eastl::string message)
	{
		printf("%s\n", message.c_str());
	}
}


