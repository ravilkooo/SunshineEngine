# Root paths
set(EDITOR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(EDITOR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Tell CMake to reconfigure if files change
set_property(GLOBAL PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${EDITOR_SRC_DIR}/*.cpp"
    "${EDITOR_INCLUDE_DIR}/*.h"
)

# Collect sources and headers
file(GLOB_RECURSE EDITOR_SOURCES "${EDITOR_SRC_DIR}/*.cpp")
file(GLOB_RECURSE EDITOR_HEADERS "${EDITOR_INCLUDE_DIR}/*.h")

add_executable(SunshineEditor
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS}
)

target_link_libraries(SunshineEditor PUBLIC SunshineEngine)

# target_compile_definitions(SunshineEngine PUBLIC EASTL_USER_DEFINED_ALLOCATOR=1)

# Link to SunshineEngine.dll
target_link_libraries(SunshineEditor
    PRIVATE
        SunshineEngine
        imgui
	    sol2_ImGui_Bindings
        FMOD
)

# Specify public include directory for clients
target_include_directories(SunshineEditor PUBLIC "${EDITOR_INCLUDE_DIR}")

# In Engine/CMakeLists.txt
target_compile_definitions(SunshineEditor PUBLIC EDITOR_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

# Collect all HLSL shader files
file(GLOB_RECURSE SHADERS "Assets/Shaders/*.hlsl")

# Add shaders as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SHADERS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add shaders to the target as "header only" so VS sees them
target_sources(SunshineEditor PRIVATE ${SHADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHADERS})

# Collect all Lua scripts files
file(GLOB_RECURSE SCRIPTS "Assets/Scripts/*.lua")

# Add scripts as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SCRIPTS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add scripts to the target as "header only" so VS sees them
target_sources(SunshineEditor PRIVATE ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
