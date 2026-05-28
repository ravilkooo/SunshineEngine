#pragma once
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <sol/sol.hpp>

struct ScriptComponent 
{
	sol::table self;
	struct 
	{
		sol::function start;
		sol::function update;
		sol::function destroy;
	};
};