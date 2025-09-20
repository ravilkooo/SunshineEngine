cmake_minimum_required (VERSION 3.20)
project (
  "ViennaGameJobSystem"
  VERSION 1.0
  DESCRIPTION "A C++20 job system for games."
  LANGUAGES CXX
)

# enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# enable clang-tidy. disable to speed up compilation
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=readability-*;cppcoreguidelines-*;-header-filter=.*; -p build)

# Tell CMake that ViennaGameJobSystem is header-only
add_library(ViennaGameJobSystem INTERFACE)
# Expose its include folder(s)
target_include_directories(ViennaGameJobSystem INTERFACE
    include
    extern/ViennaStrongType
)

add_subdirectory(extern)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  include(CTest)
  add_subdirectory(tests)
endif ()


add_library(VGJS STATIC EXCLUDE_FROM_ALL)
set_target_properties(VGJS PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB_RECURSE VGJS_HEADERS
  "include/**.h"
  "extern/**.h")

target_sources(VGJS PRIVATE ${VGJS_HEADERS})

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  PREFIX "Header Files"
  FILES ${VGJS_HEADERS}
  )
