#define RETRO_WINDOW_CAPTION "Retro Game"

#include "retro.c"

static Font           FONT_NEOSANS;
static Bitmap         SPRITESHEET;
static Animation      ANIMATEDSPRITE_QUOTE_IDLE;
static Animation      ANIMATEDSPRITE_QUOTE_WALK;

typedef enum 
{
  AC_UP,
  AC_RIGHT,
  AC_DOWN,
  AC_LEFT,
  AC_ACTION,
  AC_CANCEL
} Actions;


typedef enum
{
  PF_Idle,
  PF_Walking
} PlayerState;

AnimatedSpriteObject player;

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

  Font_Load("NeoSans.png", &FONT_NEOSANS, Colour_Make(0,0,255), Colour_Make(255,0,255));
  Bitmap_Load("cave.png", &SPRITESHEET, 0);

  Animation_LoadHorizontal(&ANIMATEDSPRITE_QUOTE_IDLE, &SPRITESHEET, 1, 100, 0, 80, 16, 16);
  Animation_LoadHorizontal(&ANIMATEDSPRITE_QUOTE_WALK, &SPRITESHEET, 4, 120,  0, 80, 16, 16);

}

void Start()
{
  AnimatedSpriteObject_Make(&player, &ANIMATEDSPRITE_QUOTE_WALK, 10, 10);
  AnimatedSpriteObject_PlayAnimation(&player, true, true);
}

void Step()
{
  if (Input_GetActionDown(AC_UP))
    player.y--;
  else if (Input_GetActionDown(AC_DOWN))
    player.y++;

  if (Input_GetActionDown(AC_LEFT))
    player.x--;
  else if (Input_GetActionDown(AC_RIGHT))
    player.x++;

  Canvas_PlaceAnimated(&player, true);
  //Canvas_PrintF(x, y, &FONT_NEOSANS, 2, "Hello World");
  Canvas_Debug(&FONT_NEOSANS);
}
