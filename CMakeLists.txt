cmake_minimum_required(VERSION 3.20)
project(Sunshine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # Good for shared libs

# Set output folders (binaries go into bin/, libs into lib/)
# Set before add_subdirectory()!!!
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # Match VS default: Unicode build, no min/max macro pollution
    add_compile_definitions(UNICODE _UNICODE NOMINMAX)
endif()

# Engine (DLL)
add_subdirectory(SunshineEngine)

# Editor (EXE)
add_subdirectory(SunshineEditor)

add_subdirectory(ThirdParty)

add_subdirectory(SunshineLibs)
