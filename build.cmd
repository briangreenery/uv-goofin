@echo off
setlocal EnableDelayedExpansion

set "SOURCE_DIR=%~dp0"

pushd "%SOURCE_DIR%\libuv-1.9.1"
call vcbuild.bat
call vcbuild.bat release x64
popd

cmake -G "Visual Studio 12 2013 Win64" "%SOURCE_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build . --config Release --target ALL_BUILD
if %errorlevel% neq 0 exit /b %errorlevel%
