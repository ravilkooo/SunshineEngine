# Root paths
set(SB_IMGUI_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SB_IMGUI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${SB_IMGUI_SRC_DIR}/*.cpp"
    "${SB_IMGUI_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE SB_IMGUI_SOURCES "${SB_IMGUI_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SB_IMGUI_HEADERS "${SB_IMGUI_INCLUDE_DIR}/*.h")

add_executable(Sandbox_Imgui
    ${SB_IMGUI_SOURCES}
    ${SB_IMGUI_HEADERS}
)

# source_group("Source Files" FILES ${EDITOR_SOURCES})
# source_group("Header Files" FILES ${EDITOR_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Sandbox_Imgui PRIVATE SunshineEngine)

# We can `#include` files from include dir
target_include_directories(Sandbox_Imgui PUBLIC include)

# Link to SunshineEngine.dll
target_link_libraries(Sandbox_Imgui
    PRIVATE
        SunshineEngine
        imgui
        FMOD
	sol2_ImGui_Bindings
)

# In Engine/CMakeLists.txt
target_compile_definitions(Sandbox_Imgui PUBLIC SANDBOX_IMGUI_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
