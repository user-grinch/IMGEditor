@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild IMGEditor.sln /property:Configuration=Release /t:IMGEditorApp  
