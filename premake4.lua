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

      project "LibRetro"
          kind            "SharedLib"
          language        "C"
          objdir          "_build"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI" }
          defines         { "RETRO_COMPILING_AS_LIBRARY" }
          links           { "SDL2" }
          includedirs     { "ref/SDL2/include", "ref/" }
          libdirs         { "ref/SDL2/lib/x86/" }
          
          files           { "retro.c", "retro.h", "ref/*.c", "ref/*.h" }
          excludes        { "ref/*.c", "ref/*.h" }

      --------------------------------------------------------------------------

      project "LibGame"
          kind            "SharedLib"
          language        "C"
          objdir          "_build"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI" }
          defines         { "RETRO_USING_RETRO_AS_LIBRARY" }
          links           { "LibRetro" }
          includedirs     { }
          libdirs         { }
          
          files           { "retro.h", "main.c", "resources.rc", "resources.rc", "assets/*.png", "assets/*.wav", "assets/*.mod" }
          excludes        { }

      --------------------------------------------------------------------------

      project "LibEditor"
          kind            "SharedLib"
          language        "C"
          objdir          "_build"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI" }
          defines         { "RETRO_USING_AS_LIBRARY" }
          links           { "LibRetro" }
          includedirs     { }
          libdirs         { }
          
          files           { "retro.h", "editor/*.c", "editor/*.h", "editor/editor_resources.rc", "editor/assets/*.png", "editor/assets/*.wav", "editor/assets/*.mod" }
          excludes        { "editor/editor_main.c" }

      --------------------------------------------------------------------------

      project "Editor"
          kind            "ConsoleApp"
          language        "C"
          objdir          "_build"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI", "WinMain" }
          defines         { "RETRO_USING_AS_LIBRARY" }
          links           { "LibRetro" }
          includedirs     { }
          libdirs         { }
          
          files           { "retro.h", "editor/editor_main.c" }
          excludes        { }

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
          
          files           { "retro.c", "retro.h", "*.c", "*.h", "ref/*.c", "ref/*.h", "resources.rc", "resources.rc", "assets/*.png", "assets/*.wav", "assets/*.mod" }
          excludes        { "retro.c", "ref/*.c", "ref/*.h" }

      --------------------------------------------------------------------------

      startproject "Editor"
      
      --------------------------------------------------------------------------
