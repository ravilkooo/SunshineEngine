# Editor project
set(EDITOR_SOURCES
    src/main.cpp
    src/TestCube.cpp
    src/DeferredGame.cpp
    src/common.cpp
    src/common_platform.cpp
    src/play_sound.cpp
)

set(EDITOR_HEADERS
    include/TestCube.h
    include/DeferredGame.h
    include/common.h
    include/common_platform.h
)

add_executable(Sandbox_Editor
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS}
)

# source_group("Source Files" FILES ${EDITOR_SOURCES})
# source_group("Header Files" FILES ${EDITOR_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox_Editor PRIVATE SunshineEngine)

# We can `#include` files from include dir
target_include_directories(Sandbox_Editor PUBLIC include)

# Link to SunshineEngine.dll
target_link_libraries(Sandbox_Editor
    PRIVATE
        SunshineEngine
        imgui
        FMOD
)

# In Engine/CMakeLists.txt
target_compile_definitions(Sandbox_Editor PUBLIC SANDBOX_EDITOR_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
