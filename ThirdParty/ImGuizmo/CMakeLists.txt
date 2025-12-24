# Create a static library for ImGuizmo
add_library(ImGuizmo STATIC
    ImGuizmo.h
    ImGuizmo.cpp

    # ImCurveEdit.h
    # ImCurveEdit.cpp
    # ImSequencer.h
    # ImSequencer.cpp
    # ImZoomSlider.h
)

# Include directories
target_include_directories(ImGuizmo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link with ImGui
target_link_libraries(ImGuizmo PRIVATE imgui)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
)
