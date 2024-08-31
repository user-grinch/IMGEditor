@echo off
set "projectdir=%CD%"

:find_vs_path
set "vs_path="
for %%d in (Community Preview Professional) do (
    for /d %%e in ("C:\Program Files\Microsoft Visual Studio\2022\%%d") do (
        if exist "%%e\Common7\Tools\VsDevCmd.bat" (
            set "vs_path=%%e"
            goto :vs_install_found
        )
    )
)
goto :eof

:vs_install_found
if not defined vs_path (
    echo Error: Visual Studio installation not found.
    exit /b 1
)

:run_premake
cd "%projectdir%\tools"
premake5.exe vs2022

:run_dev
cd "%projectdir%\build"
call %vs_path%"\Common7\Tools\VsDevCmd.bat"