cmake_minimum_required(VERSION 3.20)

project(SunshineGraphics LANGUAGES CXX)

# Root paths
# set(GF_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
# set(GF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GF_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(GF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${GF_SRC_DIR}/*.cpp"
    "${GF_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE GF_SOURCES "${GF_SRC_DIR}/*.cpp")
file(GLOB_RECURSE GF_HEADERS "${GF_INCLUDE_DIR}/*.h")

# Create the shared library
add_library(SunshineGraphics STATIC
    ${GF_SOURCES}
    ${GF_HEADERS}
)

# Set include directories (so others can `#include <SunshineGraphics/...>`)
target_include_directories(SunshineGraphics PUBLIC
    "${GF_INCLUDE_DIR}"
)

# SunshineGraphics assets dir
target_compile_definitions(SunshineGraphics PUBLIC GRAPHICS_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

# C++ standard
target_compile_features(SunshineGraphics PUBLIC cxx_std_20)

# If you want to export symbols properly on Windows:
if(WIN32)
    target_compile_definitions(SunshineGraphics PRIVATE SUNSHINEGRAPHICS_EXPORTS)
    target_link_libraries(SunshineGraphics PUBLIC
        d3d11 dxgi d3dcompiler dxguid
    )
endif()


# Link to system DirectX libraries
target_link_libraries(SunshineGraphics PUBLIC
    d3d11
    dxgi
    d3dcompiler
    dxguid
)

# Collect all HLSL shader files
file(GLOB_RECURSE SHADERS "Assets/Shaders/*.hlsl")

# Add shaders to the project so they appear in VS
source_group("Assets\\Shaders" FILES ${SHADERS})

# Add shaders as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SHADERS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add shaders to the target as "header only" so VS sees them
target_sources(SunshineGraphics PRIVATE ${SHADERS})

# DirectXTK integration
target_link_libraries(SunshineGraphics PUBLIC DirectXTK)

# assimp integration
target_link_libraries(SunshineGraphics PUBLIC assimp)

# Link EASTL where needed
target_link_libraries(SunshineGraphics PUBLIC EASTL)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
