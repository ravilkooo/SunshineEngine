cmake_minimum_required(VERSION 3.20)
project(Sunshine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Good for shared libs

# Force all targets to use dynamic runtime (/MD, /MDd)
if(MSVC)
    # Fallback for older EASTL or CMake logic
    add_compile_options(
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
    )
endif()

# Set output folders (binaries go into bin/, libs into lib/)
# Set before add_subdirectory()!!!
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # Match VS default: Unicode build, no min/max macro pollution
    add_compile_definitions(UNICODE _UNICODE NOMINMAX)
    add_compile_options(/MP)
endif()

# Engine (DLL)
add_subdirectory(SunshineEngine)

# Editor (EXE)
add_subdirectory(SunshineEditor)

add_subdirectory(ThirdParty)

# Projects path
target_compile_definitions(SunshineEditor PUBLIC PROJECTS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Projects/")