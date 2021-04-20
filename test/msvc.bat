
@echo off

set EXE_NAME="platform"

IF NOT EXIST bin mkdir bin
pushd bin

set VC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
if [%VisualStudioVersion%] == [] echo WARNING: VC cold. && call %VC_PATH%"\vcvarsall.bat" x64 >NUL

set COMPILER_FLAGS=-nologo -Fe%EXE_NAME% -fp:fast -EHa- -Gm- -GR- -Oi -WX -W4 -wd4996 -wd4100 -wd4189 -wd4127 -wd4201 -wd4101 -wd4505 -wd4204 -wd4221 -wd4200
set COMPILER_FLAGS=%COMPILER_FLAGS% -MTd -Od -Z7
rem set COMPILER_FLAGS=%COMPILER_FLAGS% -MT -O2 -Z7

set LINKER_FLAGS=-incremental:no -opt:ref

cl %COMPILER_FLAGS% ^
 -I "../../include/"^
 "../../test/main.c"^
 "../../src/platform_win32.c"^
 -link %LINKER_FLAGS%

if [%ERRORLEVEL%] EQU [0] %EXE_NAME%.exe

popd
