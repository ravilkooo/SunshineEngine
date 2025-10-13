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

add_subdirectory(imgui)

add_subdirectory(lua)
add_subdirectory(FMOD)
# add_subdirectory(ozz-animation)
add_subdirectory(sol2)

# set_target_properties(EABase EASTL assimp DirectXTK imgui JoltPhysics ViennaGameJobSystem
set_target_properties(EABase EASTL assimp DirectXTK Jolt VGJS imgui lua FMOD sol2
    PROPERTIES
    FOLDER "ThirdPartyLibs"
)   