# ThirdParty/CMakeLists.txt

# EABase
add_subdirectory(EABase)

# EASTL
set(EASTL_BUILD_BENCHMARK OFF CACHE BOOL "Disable EASTL benchmarks")
set(EASTL_BUILD_TESTS OFF CACHE BOOL "Disable EASTL tests")
add_subdirectory(EASTL)

add_subdirectory(DirectXTK)
add_subdirectory(assimp)

add_subdirectory(JoltPhysics/Build)
add_subdirectory(ViennaGameJobSystem)

# add_subdirectory(imgui)
# add_subdirectory(ozz-animation)

# set_target_properties(EABase EASTL assimp DirectXTK imgui JoltPhysics ViennaGameJobSystem
set_target_properties(EABase EASTL assimp DirectXTK Jolt VGJS
    PROPERTIES
    FOLDER "ThirdPartyLibs"
)   