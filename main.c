#define RETRO_WINDOW_CAPTION "Retro Game"

#include "retro.c"

static Font neoSans;

void Init(Settings* settings)
{
  settings->windowWidth = 1280;
  settings->windowHeight = 720;

  Palette_Make(&settings->palette);
  Palette_LoadFromBitmap("palette.png", &settings->palette);

  Input_BindKey(SDL_SCANCODE_W, 'UP');
  Input_BindKey(SDL_SCANCODE_D, 'RIGH');
  Input_BindKey(SDL_SCANCODE_S, 'DOWN');
  Input_BindKey(SDL_SCANCODE_A, 'LEFT');

  Input_BindKey(SDL_SCANCODE_RETURN, 'ACTI');
  Input_BindKey(SDL_SCANCODE_ESCAPE, 'CANC');

  Font_Load("NeoSans.png", &neoSans, Colour_Make(0,0,255), Colour_Make(255,0,255));
}

void Start()
{
}

void Step()
{
  Canvas_PrintStr(20, 20, &neoSans, 2, "Hello World");
  Canvas_Debug(&neoSans);
}
