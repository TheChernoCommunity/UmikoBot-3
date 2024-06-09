@echo off

set originalDir=%CD%
set scriptDir=%~dp0
set projectRoot=%scriptDir%\..

cd "%projectRoot%\bin" >nul 2>nul && del UmikoBot-3.exe

IF NOT EXIST "%projectRoot%\build" mkdir "%projectRoot%\build"
cd "%projectRoot%\build"

rem Pick one.
rem cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 ..
cmake -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Debug -G "NMake Makefiles" ..
rem cmake -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Debug -G "Ninja" ..

if errorlevel 1 (
    cd %originalDir%
    exit /B
)

cmake --build .
if errorlevel 1 (
    cd %originalDir%
    exit /B
)

echo.
cd "%projectRoot%\bin\"
UmikoBot-3.exe

cd %originalDir%
