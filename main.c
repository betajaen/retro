#define RETRO_WINDOW_CAPTION "Retro Game"

#include "retro.c"

static Font neoSans;

typedef enum 
{
  AC_UP,
  AC_RIGHT,
  AC_DOWN,
  AC_LEFT,
  AC_ACTION,
  AC_CANCEL
} Actions;

void Init(Settings* settings)
{
  settings->windowWidth = 1280;
  settings->windowHeight = 720;

  Palette_Make(&settings->palette);
  Palette_LoadFromBitmap("palette.png", &settings->palette);

  Input_BindKey(SDL_SCANCODE_W, AC_UP);
  Input_BindKey(SDL_SCANCODE_D, AC_RIGHT);
  Input_BindKey(SDL_SCANCODE_S, AC_DOWN);
  Input_BindKey(SDL_SCANCODE_A, AC_LEFT);

  Input_BindKey(SDL_SCANCODE_RETURN, AC_ACTION);
  Input_BindKey(SDL_SCANCODE_ESCAPE, AC_CANCEL);

  Font_Load("NeoSans.png", &neoSans, Colour_Make(0,0,255), Colour_Make(255,0,255));
}

void Start()
{
}

int x = 20, y = 20;

void Step()
{
  if (Input_GetActionDown(AC_UP))
    y--;
  else if (Input_GetActionDown(AC_DOWN))
    y++;

  if (Input_GetActionDown(AC_LEFT))
    x--;
  else if (Input_GetActionDown(AC_RIGHT))
    x++;

  Canvas_PrintF(x, y, &neoSans, 2, "Hello World");
  Canvas_Debug(&neoSans);
}
