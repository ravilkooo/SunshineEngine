set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_library(SunshineEngine STATIC
    ${SE_SOURCES}
    ${SE_HEADERS}
)

set_target_properties(SunshineEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# target_compile_definitions(SunshineEngine PUBLIC EASTL_USER_DEFINED_ALLOCATOR=1)

# Define ENGINE_EXPORTS when compiling Engine itself
# target_compile_definitions(SunshineEngine PRIVATE SUNSHINE_ENGINE_EXPORTS)

# Specify public include directory for clients
target_include_directories(SunshineEngine PUBLIC "${SE_INCLUDE_DIR}")

target_link_libraries(SunshineEngine
    PUBLIC
        SunshineGraphics
        assimp
        ViennaGameJobSystem
        lua
        FMOD
        sol2
        EASTL
)

message(STATUS "FMOD_LIB_DIR (from engine) = ${FMOD_LIB_DIR}")

add_custom_command(TARGET SunshineEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL.dll>
        $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod.dll>
        $<TARGET_FILE_DIR:SunshineEngine>
)

message(STATUS $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/bin/Debug> $<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/bin/Release> )

add_custom_command(TARGET SunshineEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL.dll>
        $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod.dll>
        ${CMAKE_BINARY_DIR}/bin
        $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/bin/Debug>
        $<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/bin/Release> 
)


# -------------- JoltPhysics --------------

target_link_libraries(SunshineEngine PUBLIC Jolt)

# -------------- JoltPhysics --------------

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
