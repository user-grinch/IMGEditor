@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild IMGEditor.sln /property:Configuration=Debug /t:IMGEditorApp
