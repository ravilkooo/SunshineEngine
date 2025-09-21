cmake_minimum_required(VERSION 3.20)
project(FMOD LANGUAGES CXX)

# FMOD (Core)
set(FMOD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(FMOD_INCLUDE_DIR "${FMOD_ROOT}/api/inc")
set(FMOD_LIB_DIR "${FMOD_ROOT}/api/lib/x64" CACHE PATH "FMOD library dir")

# Define imported FMOD target
add_library(FMOD INTERFACE)

# Add include dir
target_include_directories(FMOD INTERFACE ${FMOD_INCLUDE_DIR})

# -----------------

# Link proper .lib (Debug vs Release)
target_link_libraries(FMOD INTERFACE
    $<$<CONFIG:Debug>:${FMOD_LIB_DIR}/fmodL_vc.lib>
    $<$<CONFIG:Release>:${FMOD_LIB_DIR}/fmod_vc.lib>
)
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
)
