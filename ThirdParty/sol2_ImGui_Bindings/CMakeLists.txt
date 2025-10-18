cmake_minimum_required(VERSION 3.15)

project(
        sol2_ImGui_Bindings
        VERSION 1.91.8
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Define the Library Target ---
add_library(sol2_ImGui_Bindings STATIC)
add_library(sol2_ImGui_Bindings::sol2_ImGui_Bindings ALIAS sol2_ImGui_Bindings)

# --- Source Files ---
target_sources(sol2_ImGui_Bindings PRIVATE
        sol_ImGui.cpp
        sol_ImGui_Enums.cpp
        sol_ImGui_Windows.cpp
        sol_ImGui_Input.cpp
        sol_ImGui_Layout.cpp
        sol_ImGui_Popups.cpp
        sol_ImGui_Tables.cpp
        sol_ImGui_Widgets.cpp
        sol_ImGui_Utility.cpp
)

# --- Include Directories ---
target_include_directories(sol2_ImGui_Bindings
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# --- Dependency Management ---
target_link_libraries(sol2_ImGui_Bindings
    PUBLIC
        imgui
        sol2
        lua
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
)