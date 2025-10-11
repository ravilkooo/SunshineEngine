// SunshineEngineAPI.h
#pragma once
#include "Core/EASTLAllocHooks.h"

#ifdef SUNSHINE_ENGINE_EXPORTS
#   define SUNSHINE_ENGINE_API __declspec(dllexport)
#else
// #   define SUNSHINE_ENGINE_API __declspec(dllimport)
#   define SUNSHINE_ENGINE_API 
#endif

