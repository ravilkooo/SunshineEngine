# Editor project
set(EDITOR_SOURCES
    src/main.cpp
    src/TestCube.cpp
    src/DeferredGame.cpp
)

set(EDITOR_HEADERS
    include/TestCube.h
    include/DeferredGame.h
)

add_executable(SunshineEditor
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS}
)

source_group("Source Files" FILES ${EDITOR_SOURCES})
source_group("Header Files" FILES ${EDITOR_HEADERS})

target_link_libraries(SunshineEditor PRIVATE SunshineEngine)

# We can `#include` files from include dir
target_include_directories(SunshineEditor PUBLIC include)

# Link to SunshineEngine.dll
target_link_libraries(SunshineEditor PRIVATE SunshineEngine)

# In Engine/CMakeLists.txt
target_compile_definitions(SunshineEditor PUBLIC EDITOR_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
