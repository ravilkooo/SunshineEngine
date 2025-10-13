add_subdirectory(SunshineGraphics)
add_subdirectory(SunshineResources)

set_target_properties(SunshineGraphics SunshineResources
    PROPERTIES
    FOLDER "SunshineLibs"
)