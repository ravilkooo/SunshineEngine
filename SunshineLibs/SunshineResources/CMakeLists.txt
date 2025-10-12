cmake_minimum_required(VERSION 3.20)

project(SunshineResources LANGUAGES CXX)

# Root paths
set(SR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
# set(SR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# set(SR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${SR_SRC_DIR}/*.cpp"
    "${SR_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE SR_SOURCES "${SR_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SR_HEADERS "${SR_INCLUDE_DIR}/*.h")

# Create the shared library
add_library(SunshineResources STATIC
    ${SR_SOURCES}
    ${SR_HEADERS}
)

# Set include directories (so others can `#include <SunshineResources/...>`)
target_include_directories(SunshineResources PUBLIC
    "${SR_INCLUDE_DIR}"
)

# C++ standard
target_compile_features(SunshineResources PUBLIC cxx_std_20)

# If you want to export symbols properly on Windows:
if(WIN32)
    target_compile_definitions(SunshineResources PRIVATE SunshineResources_EXPORTS)
endif()

# Link EASTL where needed
target_link_libraries(SunshineResources PUBLIC EASTL)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
