@echo off

set originalDir=%CD%
set scriptDir=%~dp0
set projectRoot=%scriptDir%\..

rmdir /S /Q "%projectRoot%\bin" "%projectRoot%\build" >nul 2>nul
echo Removed build/ and bin/ directories.
