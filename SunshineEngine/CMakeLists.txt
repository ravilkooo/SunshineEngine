set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Root paths
set(SE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROJECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Projects")

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

# ---------- Graphics ----------

# Link to system DirectX libraries
target_link_libraries(SunshineEngine PUBLIC
    d3d11
    d3d12
    dxgi
    d3dcompiler
    dxguid
)

# DirectXTK integration
target_link_libraries(SunshineEngine PUBLIC DirectXTK)

# -------- nlohmann_json ---------

# find_package(nlohmann_json 3.12.0 REQUIRED)
set(JSON_BuildTests OFF CACHE INTERNAL "")
target_link_libraries(SunshineEngine PUBLIC nlohmann_json::nlohmann_json)

# ---------- Other Libs ----------

# target_compile_definitions(SunshineEngine PUBLIC EASTL_USER_DEFINED_ALLOCATOR=1)

# Define ENGINE_EXPORTS when compiling Engine itself
# target_compile_definitions(SunshineEngine PRIVATE SUNSHINE_ENGINE_EXPORTS)

# Specify public include directory for clients
target_include_directories(SunshineEngine PUBLIC "${SE_INCLUDE_DIR}")

target_link_libraries(SunshineEngine
    PUBLIC
        assimp
        ViennaGameJobSystem
        imgui
        lua
        FMOD
        sol2
        EASTL
)

message(STATUS "FMOD_LIB_DIR (from engine) = ${FMOD_LIB_DIR}")

# add_custom_command(TARGET SunshineEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL.dll>
#         $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod.dll>
#         $<TARGET_FILE_DIR:SunshineEngine>
# )

# message(STATUS $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/bin/Debug> $<$<CONFIG:Release>:${CMAKE_BINARY_DIR}/bin/Release> )

# add_custom_command(TARGET SunshineEngine POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL.dll ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/bin/Debug>
#         $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod.dll ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/bin/Release>
# )


# -------------- JoltPhysics --------------

target_link_libraries(SunshineEngine PUBLIC Jolt)

# -------------- Assets --------------

target_compile_definitions(SunshineEngine PUBLIC ENGINE_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")
target_compile_definitions(SunshineEngine PUBLIC PROJECT_DIR=L"${PROJECTS_DIR}/")

# Collect all HLSL shader files
file(GLOB_RECURSE SHADERS "Assets/Shaders/*.hlsl")

# Add shaders as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SHADERS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add shaders to the target as "header only" so VS sees them
target_sources(SunshineEngine PRIVATE ${SHADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHADERS})

# Collect all Lua scripts files
file(GLOB_RECURSE SCRIPTS "Assets/Scripts/*.lua")

# Add scripts as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SCRIPTS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add scripts to the target as "header only" so VS sees them
target_sources(SunshineEngine PRIVATE ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
