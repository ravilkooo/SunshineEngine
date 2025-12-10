#pragma once

#include <sol/sol.hpp>
#include <EASTL/vector.h>
#include <EASTL/functional.h>

namespace AutoBindings {
    using Binder = eastl::function<void(sol::state&)>;

    // Add a binder to be executed during scripting registration
    void AddBinder(Binder b);

    // Execute all registered binders
    void RegisterAll(sol::state& lua);
}

// Utility empty APPLY macro for places where a component has no fields/methods
#ifndef LUA_EMPTY_APPLY
#define LUA_EMPTY_APPLY(F)
#endif

// One-liner macro to self-register a component's Lua bindings in its .cpp
// FieldsPairs: macro that expands to comma-separated pairs "name, &TYPE::member" (or empty)
// MethodsPairs: macro that expands to comma-separated pairs "luaName, callable" (or empty)
// GetterName: the name of the getter on GameObject (e.g., "getTransform")
#define LUA_REGISTER_COMPONENT(TYPE, LuaTypeName, FieldsPairs, MethodsPairs, GetterName) \
    namespace { \
        struct TYPE##__LuaAutoReg { \
            TYPE##__LuaAutoReg() { \
                AutoBindings::AddBinder([](sol::state& lua){ \
                    /* Register usertype with fields and methods */ \
                    lua.new_usertype<TYPE>(LuaTypeName, FieldsPairs MethodsPairs); \
                    /* Attach GameObject getter */ \
                    lua["GameObject"][GetterName] = &ScriptingBindings::GO_Get<TYPE>; \
                }); \
            } \
        } TYPE##__LuaAutoReg_Instance; \
    }

//Register singleton
#define LUA_REGISTER_TYPE(TYPE, LuaTypeName, FieldPairs, MethodPairs) \
    namespace { \
        struct TYPE##__LuaAutoReg { \
            TYPE##__LuaAutoReg() { \
                AutoBindings::AddBinder([](sol::state& lua){ \
                    lua.new_usertype<TYPE>(LuaTypeName, sol::no_constructor, FieldPairs MethodPairs); \
                }); \
            } \
        } TYPE##__LuaAutoReg_Instance; \
    }

