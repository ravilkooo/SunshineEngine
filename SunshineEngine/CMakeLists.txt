# Root paths
set(SE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${SE_SRC_DIR}/*.cpp"
    "${SE_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE SE_SOURCES "${SE_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SE_HEADERS "${SE_INCLUDE_DIR}/*.h")

add_library(SunshineEngine SHARED
    ${SE_SOURCES}
    ${SE_HEADERS}
)

set_target_properties(SunshineEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# Define ENGINE_EXPORTS when compiling Engine itself
target_compile_definitions(SunshineEngine PRIVATE SUNSHINE_ENGINE_EXPORTS)

# Specify public include directory for clients
target_include_directories(SunshineEngine PUBLIC "${SE_INCLUDE_DIR}")

# Public headers (installed/exported)
# target_include_directories(SunshineEngine
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

# Sources
target_sources(SunshineEngine PRIVATE
    src/SunshineEngine.cpp
)

target_link_libraries(SunshineEngine
    PUBLIC
        SunshineGraphics
        assimp
        EASTL
        Jolt
        ViennaGameJobSystem
        lua
        FMOD
        sol2
)

message(STATUS "FMOD DLL DIR = ${FMOD_DLL_DIR}")

add_custom_command(TARGET SunshineEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL.dll>
        $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod.dll>
        $<TARGET_FILE_DIR:SunshineEngine>
)

target_compile_definitions(SunshineEngine PUBLIC ENGINE_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

# Collect all Lua scripts files
file(GLOB_RECURSE SCRIPTS "Assets/Scripts/*.lua")

# Add scripts to the project so they appear in VS
source_group("Assets\\Scripts" FILES ${SCRIPTS})

# Add scripts as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SCRIPTS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add scripts to the target as "header only" so VS sees them
target_sources(SunshineEngine PRIVATE ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
