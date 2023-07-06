@echo off

mkdir "%~dp0..\build"
pushd "%~dp0..\build"
cl -FC -Zi "%~dp0\..\compiler.cpp" user32.lib
popd

@REM cd is current executing dir, ~dp0 is batch file dir
@REM https://stackoverflow.com/questions/4419868/what-is-the-current-directory-in-a-batch-file
@REM echo %cd%
@REM echo %~dp0
