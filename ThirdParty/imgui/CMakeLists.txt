# Create a static library for ImGui
add_library(imgui STATIC
    imconfig.h
    imgui_internal.h
    imgui.h

    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp

    imgui_demo.cpp

    backends/imgui_impl_dx11.cpp
    backends/imgui_impl_win32.cpp

    misc/cpp/imgui_stdlib.h
    misc/cpp/imgui_stdlib.cpp
)

# Include directories
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
)