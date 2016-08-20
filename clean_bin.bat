if not exist bin mkdir bin
del bin\*.exe
del bin\*.dll
del bin\*.exp
del bin\*.ilk
del bin\*.lib
del bin\*.pdb
del bin\*.idb

IF NOT EXIST "bin\SDL2.dll" COPY "ref\SDL2\lib\x86\SDL2.dll" "bin\SDL2.dll"
