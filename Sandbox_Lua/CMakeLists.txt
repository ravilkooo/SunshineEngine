# Editor project

# Root paths
set(SB_LUA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SB_LUA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${SB_LUA_SRC_DIR}/*.cpp"
    "${SB_LUA_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE SB_LUA_SOURCES "${SB_LUA_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SB_LUA_HEADERS "${SB_LUA_INCLUDE_DIR}/*.h")

add_executable(Sandbox_Lua
    ${SB_LUA_SOURCES}
    ${SB_LUA_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox_Lua PRIVATE SunshineEngine)

# We can `#include` files from include dir
target_include_directories(Sandbox_Lua PUBLIC include)

# Link to SunshineEngine.dll
target_link_libraries(Sandbox_Lua
    PRIVATE
        SunshineEngine
        imgui
        FMOD
)

# In Engine/CMakeLists.txt
target_compile_definitions(Sandbox_Lua PUBLIC SANDBOX_LUA_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
