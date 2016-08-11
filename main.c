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
Point playerVelocity;

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
  AnimatedSpriteObject_Make(&player, &ANIMATEDSPRITE_QUOTE_WALK, Canvas_GetWidth() / 2, Canvas_GetHeight() / 2);
  AnimatedSpriteObject_PlayAnimation(&player, true, true);

  playerVelocity.x = 0;
  playerVelocity.y = 0;
}

void Step()
{

  if (Input_GetActionDown(AC_UP))
    playerVelocity.y -= 1;
  else if (Input_GetActionDown(AC_DOWN))
    playerVelocity.y += 1;

  if (Input_GetActionDown(AC_LEFT))
    playerVelocity.x--;
  else if (Input_GetActionDown(AC_RIGHT))
    playerVelocity.x++;
  else
  {
    if (playerVelocity.x < 0)
      playerVelocity.x++;
    else if (playerVelocity.x > 0)
      playerVelocity.x--;
  }

  if (playerVelocity.x < -8)
    playerVelocity.x = -8;
  else if (playerVelocity.x > 8)
    playerVelocity.x = 8;

  player.x += playerVelocity.x;

  if (playerVelocity.x > 0)
    player.flags &= ~SOF_FlipX;
  else if (playerVelocity.x < 0)
    player.flags |= SOF_FlipX;

  if (player.x < 0)
  {
    player.x = 0;
    playerVelocity.x = 0;
  }
  else if (player.x + player.animation->w > Canvas_GetWidth())
  {
    player.x = Canvas_GetWidth() - player.animation->w;
    playerVelocity.x = 0;
  }

  if (playerVelocity.x != 0)
  {
    if (player.animation == &ANIMATEDSPRITE_QUOTE_IDLE)
    {
      AnimatedSpriteObject_SwitchAnimation(&player, &ANIMATEDSPRITE_QUOTE_WALK, true);
    }
  }
  else
  {
    if (player.animation == &ANIMATEDSPRITE_QUOTE_WALK)
    {
      AnimatedSpriteObject_SwitchAnimation(&player, &ANIMATEDSPRITE_QUOTE_IDLE, false);
    }
  }

  Canvas_Set(0);
  player.x -= 10;
  Canvas_PlaceAnimated(&player, true);
  player.x += 10;
  Canvas_Set(1);
  Canvas_PlaceAnimated(&player, true);

  Canvas_Debug(&FONT_NEOSANS);
}
