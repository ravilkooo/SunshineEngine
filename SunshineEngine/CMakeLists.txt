add_library(SunshineEngine SHARED
    include/SunshineEngineAPI.h
)

set_target_properties(SunshineEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin"
)

# Define ENGINE_EXPORTS when compiling Engine itself
target_compile_definitions(SunshineEngine PRIVATE SUNSHINE_ENGINE_EXPORTS)

# Specify public include directory for clients
target_include_directories(SunshineEngine PUBLIC include)

# Public headers (installed/exported)
# target_include_directories(SunshineEngine
#    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

# Sources
target_sources(SunshineEngine PRIVATE
    src/SunshineEngine.cpp
)

target_link_libraries(SunshineEngine
    PUBLIC
        SunshineGraphics
        assimp
        EASTL
        Jolt
        ViennaGameJobSystem
        lua
)

target_compile_definitions(SunshineEngine PUBLIC ENGINE_ASSETS_DIR=L"${CMAKE_CURRENT_SOURCE_DIR}/Assets/")

# Collect all Lua scripts files
file(GLOB_RECURSE SCRIPTS "Assets/Scripts/*.lua")

# Add scripts to the project so they appear in VS
source_group("Assets\\Scripts" FILES ${SCRIPTS})

# Add scripts as "sources" just for IDE visibility, not for compilation
set_property(SOURCE ${SCRIPTS} PROPERTY HEADER_FILE_ONLY TRUE)

# Add scripts to the target as "header only" so VS sees them
target_sources(SunshineEngine PRIVATE ${SCRIPTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})
