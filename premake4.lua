solution "WER"

      -- Red Game --------------------------------------------------------------

      configurations  { "Debug", "Release" }
     
      targetdir       "bin"
      debugdir        "bin"

      configuration "Debug"
        defines       { "DEBUG", "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
        flags         { "Symbols" }

      configuration "Release"
        defines       { "NDEBUG", "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
        flags         { "Optimize" }

      project "WERWindows"
          kind            "ConsoleApp"
          language        "C++"
          flags           { "FatalWarnings", "NoExceptions", "NoRTTI" }
           defines        { "RETRO_WIN" }
          links           { "SDL2", "SDL2main"}
          includedirs     { "ref/SDL2/include", "ref/" }
          libdirs         { "ref/SDL2/lib/x86/" }
          
          files           { "retro.c", "retro.h", "*.c", "*.h", "resources.rc", "*.png", "*.ogg" }
          excludes        { "retro.c" }

      --------------------------------------------------------------------------
