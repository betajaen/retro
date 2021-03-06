#define RETRO_WINDOW_CAPTION "Retro Editor"
#define RETRO_SHORTHAND
#define RETRO_NAMESPACES
#define RETRO_NO_MAIN

#include "retro.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FONT_WIDTH       7
#define FONT_HEIGHT      13
#define UI_BORDER_SIZE   2
#define UI_SHADOW_SIZE   2
#define COLOUR_WHITE     255
#define COLOUR_BLACK     254
#define COLOUR_GRAY      253
#define COLOUR_RED1      252
#define COLOUR_RED2      251
#define COLOUR_BLUE1     249
#define COLOUR_BLUE2     248

static Font FONT_UI;
static S32  UI_LastWidth = 0, UI_LastHeight = 0, UI_LastRight = 0, UI_Last_Bottom = 0;
static S32 GameContext = -1;

void Editor_SetPalette()
{
  palette.set(COLOUR_WHITE,    _RGB("FFFFFF"));  // white
  palette.set(COLOUR_BLACK,    _RGB("000000"));  // black
  palette.set(COLOUR_GRAY,     _RGB("3E3840"));  // gray
  palette.set(COLOUR_RED1,     _RGB("B44B54"));  // red-1
  palette.set(COLOUR_RED2,     _RGB("CC6773"));  // red-2
  palette.set(COLOUR_BLUE1,    _RGB("60A4BF"));  // blue-1
  palette.set(COLOUR_BLUE2,    _RGB("86BAD0"));  // blue-2

  resources.loadFontFixed("font_ui", &FONT_UI, FONT_WIDTH, _RGB("0"));
}

bool Editor_Button(S32 x, S32 y, const char* str)
{
  S32 w = strlen(str) * FONT_WIDTH + UI_BORDER_SIZE * 2;
  S32 h = FONT_HEIGHT + UI_BORDER_SIZE * 2;

  canvas.drawRectangle(COLOUR_RED2, _RECT(x, y, w, h));
  canvas.drawRectangle(COLOUR_RED1, _RECT(x, y + h - UI_BORDER_SIZE, w, UI_BORDER_SIZE));

  canvas.print(x + UI_BORDER_SIZE, y + UI_BORDER_SIZE, &FONT_UI, COLOUR_WHITE, str);
  
  UI_LastWidth = w;
  UI_LastHeight = h;
  UI_LastRight = x + w;

  return true;
}


RETRO_USER_INIT_API void Init()
{
  Editor_SetPalette();

  input.bindKey(RETRO_KEY_0, 1);
}

RETRO_USER_START_API void Start()
{
  canvas.flags(0, CNF_Clear | CNF_Render, COLOUR_GRAY);
}

RETRO_USER_STEP_API void Step()
{
  UI_LastWidth = 0; UI_LastHeight = 0; UI_LastRight = 0; UI_Last_Bottom = 0;

  Editor_Button(1 + UI_LastRight, rand() % 5, "Play");
  Editor_Button(1 + UI_LastRight, rand() % 5, "Pause");
  Editor_Button(1 + UI_LastRight, rand() % 5, "Stop");

  if (input.released(1))
  {
    if (GameContext == -1)
    {
      const char* name = "LibGame";
      printf("Loading Game %s\n", name);
      GameContext = Retro_Context_LoadFromLibrary(name, 0);
      printf("Loaded Game %i\n", GameContext);
    }
    else
    {
      printf("Unloading Game %i\n", GameContext);
      Retro_Context_Unload(GameContext);
      GameContext = -1;
      printf("Unloaded Game\n");
    }
  }


  Retro_Debug(&FONT_UI);
}

int main(int argc, char *argv[])
{
  Retro_Settings settings = Retro_Default_Settings;

  settings.windowWidth   = 1280;
  settings.windowHeight  = 720;
  settings.canvasWidth   = settings.windowWidth / 2;
  settings.canvasHeight  = settings.windowHeight;
  settings.canvasScaleX  = 1;
  settings.canvasScaleY  = 1;
  settings.canvasX       = 0;

  Retro_Context_LoadFromInternal(&settings, Init, Start, Step, 0);
  Retro_Shutdown();
  return 0;
}
