
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x86

setlocal enableextensions enabledelayedexpansion
set common_c=..\main.c -nologo -D_WIN32_WINNT=0x0501 -MTd -TC -FC -EHa- -I..\ref -I..\ref\SDL2\include\
set common_l=/link /OPT:REF user32.lib gdi32.lib winmm.lib SDL2.lib SDL2main.lib resources.res /LIBPATH:..\ref\SDL2\lib\x86\ /LIBPATH:.. /SUBSYSTEM:CONSOLE

echo Building resources...
rc /nologo /fo resources.res resources.rc

if not exist bin mkdir bin
IF NOT EXIST "bin\SDL2.dll" COPY "ref\SDL2\lib\x86\SDL2.dll" "bin\SDL2.dll"

pushd bin

echo Compiling...
cl !common_c! -O2 -DRELEASE_BUILD /Gy /Oy !common_l! 

popd
