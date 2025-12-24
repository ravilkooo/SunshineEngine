#pragma once

#include "sol/sol.hpp"
#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

#include <Windows/Keys.h>



class PlayerLuaKeyActionsMapping
{
	eastl::string scriptPath;

	// Keys::??? -> Some lua function inside lua script file
	eastl::unordered_map<Keys, eastl::string> m_keyActionMapping;
};
