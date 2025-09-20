@echo off
REM =====================================================
REM Configure Sunshine project with CMake for VS2022 (x64)
REM =====================================================

REM Create build folder if it doesn’t exist
if not exist build (
    mkdir build
)

REM Move into build folder
cd build

REM Run CMake configuration
cmake .. -G "Visual Studio 17 2022" -A x64

REM Print instructions for the user
echo =====================================================
echo If configuration and generating done succesfully:
echo =====================================================
echo    Open build\Sunshine.sln to build the project.
echo    Build ALL_BUILD to build the whole solution.
echo    Set some project (e.g. SunshineEditor) as Startup Project.
echo    Run this project.
echo =====================================================
echo Else:
echo ====================================================
echo    Clear build folder and try again
echo    Or call @ravilkooo :)

pause >nul
