#define RETRO_WINDOW_CAPTION "Retro Game"

#include "retro.c"

static Font           FONT_NEOSANS;
static Bitmap         SPRITESHEET;
static Animation      ANIMATEDSPRITE_QUOTE_IDLE;
static Animation      ANIMATEDSPRITE_QUOTE_WALK;
static Sound          SOUND_COIN;

typedef enum 
{
  AC_UP,
  AC_RIGHT,
  AC_DOWN,
  AC_LEFT,
  AC_ACTION,
  AC_CANCEL,
  AC_MUSIC_ON,
  AC_MUSIC_OFF,
  AC_ARENA_LOAD,
  AC_ARENA_SAVE,
} Actions;


typedef enum
{
  PF_Idle,
  PF_Walking
} PlayerState;

typedef struct
{
  AnimatedSpriteObject player;
  Point velocity;
} GameState;

GameState* state;

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
  Input_BindKey(SDL_SCANCODE_1, AC_MUSIC_ON);
  Input_BindKey(SDL_SCANCODE_2, AC_MUSIC_OFF);
  Input_BindKey(SDL_SCANCODE_5, AC_ARENA_SAVE);
  Input_BindKey(SDL_SCANCODE_6, AC_ARENA_LOAD);

  Font_Load("NeoSans.png", &FONT_NEOSANS, Colour_Make(0,0,255), Colour_Make(255,0,255));
  Bitmap_Load("cave.png", &SPRITESHEET, 0);

  Animation_LoadHorizontal(&ANIMATEDSPRITE_QUOTE_IDLE, &SPRITESHEET, 1, 100, 0, 80, 16, 16);
  Animation_LoadHorizontal(&ANIMATEDSPRITE_QUOTE_WALK, &SPRITESHEET, 4, 120, 0, 80, 16, 16);

  Sound_Load(&SOUND_COIN, "coin.wav");
}

void Start()
{
  state = Scope_New(GameState);

  AnimatedSpriteObject_Make(&state->player, &ANIMATEDSPRITE_QUOTE_WALK, Canvas_GetWidth() / 2, Canvas_GetHeight() / 2);
  AnimatedSpriteObject_PlayAnimation(&state->player, true, true);

  state->velocity.x = 0;
  state->velocity.y = 0;

  Music_Play("origin.mod");
}

void Step()
{

  if (Input_GetActionReleased(AC_ACTION))
  {
    Sound_Play(&SOUND_COIN, 128);
  }

  if (Input_GetActionReleased(AC_MUSIC_ON))
  {
    Music_Play("origin.mod");
  }

  if (Input_GetActionReleased(AC_MUSIC_OFF))
  {
    Music_Stop();
  }

  if (Input_GetActionReleased(AC_ARENA_SAVE))
  {
    Arena_Save("arena.raw");
  }

  if (Input_GetActionReleased(AC_ARENA_LOAD))
  {
    Arena_Load("arena.raw", true);
  }

  if (Input_GetActionDown(AC_UP))
    state->velocity.y -= 1;
  else if (Input_GetActionDown(AC_DOWN))
    state->velocity.y += 1;

  if (Input_GetActionDown(AC_LEFT))
    state->velocity.x--;
  else if (Input_GetActionDown(AC_RIGHT))
    state->velocity.x++;
  else
  {
    if (state->velocity.x < 0)
      state->velocity.x++;
    else if (state->velocity.x > 0)
      state->velocity.x--;
  }

  if (state->velocity.x < -8)
    state->velocity.x = -8;
  else if (state->velocity.x > 8)
    state->velocity.x = 8;

  state->player.x += state->velocity.x;

  if (state->velocity.x > 0)
    state->player.flags &= ~SOF_FlipX;
  else if (state->velocity.x < 0)
    state->player.flags |= SOF_FlipX;

  if (state->player.x < 0)
  {
    state->player.x = 0;
    state->velocity.x = 0;
  }
  else if (state->player.x + state->player.w > Canvas_GetWidth())
  {
    state->player.x = Canvas_GetWidth() - state->player.w;
    state->velocity.x = 0;
  }

  if (state->velocity.x != 0)
  {
    if (state->player.animationHandle == ANIMATEDSPRITE_QUOTE_IDLE.animationHandle)
    {
      AnimatedSpriteObject_SwitchAnimation(&state->player, &ANIMATEDSPRITE_QUOTE_WALK, true);
    }
  }
  else
  {
    if (state->player.animationHandle == ANIMATEDSPRITE_QUOTE_WALK.animationHandle)
    {
      AnimatedSpriteObject_SwitchAnimation(&state->player, &ANIMATEDSPRITE_QUOTE_IDLE, false);
    }
  }

  Canvas_Set(0);
  state->player.x -= 10;
  Canvas_PlaceAnimated(&state->player, true);
  state->player.x += 10;
  Canvas_Set(1);
  Canvas_PlaceAnimated(&state->player, true);

  Canvas_Debug(&FONT_NEOSANS);
}
