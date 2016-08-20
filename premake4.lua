solution "Retro"

      --------------------------------------------------------------------------

      configurations      { "Debug", "Release" }
     
      targetdir           "bin"
      debugdir            "bin"

      configuration "Debug"
        defines           { "DEBUG", "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
        flags             { "Symbols" }

      configuration "Release"
        defines           { "NDEBUG", "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
        flags             { "Optimize" }

      --------------------------------------------------------------------------

      project "Game"
          kind            "ConsoleApp"
          language        "C"
          objdir          "_build"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI", "WinMain" }
          defines         { "RETRO_STANDALONE_PROGRAM" }
          links           { "SDL2", "SDL2main" }
          includedirs     { "ref/SDL2/include", "ref/" }
          libdirs         { "ref/SDL2/lib/x86/" }
          
          files           { "retro.c", "retro*.h", "*.c", "*.h", "ref/*.c", "ref/*.h", "resources.rc", "resources.rc", "assets/*.png", "assets/*.wav", "assets/*.mod" }
          excludes        { "retro.c", "ref/*.c", "ref/*.h" }

      --------------------------------------------------------------------------

      startproject "Editor"
      
      --------------------------------------------------------------------------
