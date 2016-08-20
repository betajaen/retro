#define RETRO_DEFAULT_WINDOW_CAPTION "Retro Game"

#define RETRO_NAMESPACES
#include "retro.h"

static Retro_Font                 FONT_NEOSANS;
static Retro_BitmapHandle         SPRITESHEET;
static Retro_AnimationHandle      ANIMATEDSPRITE_QUOTE_IDLE;
static Retro_AnimationHandle      ANIMATEDSPRITE_QUOTE_WALK;
static Retro_SoundHandle          SOUND_COIN;
static Retro_Timer                TIMER;

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
  Retro_AnimationObject player;
  Retro_Point velocity;
} GameState;

GameState* state;

RETRO_USER_INIT_API void Init()
{
  SPRITESHEET = resources.loadBitmap("cave.png", 0);
  SOUND_COIN  = resources.loadSound("coin.wav");
  resources.loadFont("NeoSans.png", &FONT_NEOSANS, Retro_Colour_Make(0,0,255), Retro_Colour_Make(255,0,255));

  input.bindKey(RETRO_KEY_W, AC_UP);
  input.bindKey(RETRO_KEY_D, AC_RIGHT);
  input.bindKey(RETRO_KEY_S, AC_DOWN);
  input.bindKey(RETRO_KEY_A, AC_LEFT);
  input.bindKey(RETRO_KEY_RETURN, AC_ACTION);
  input.bindKey(RETRO_KEY_ESCAPE, AC_CANCEL);
  input.bindKey(RETRO_KEY_1, AC_MUSIC_ON);
  input.bindKey(RETRO_KEY_2, AC_MUSIC_OFF);
  input.bindKey(RETRO_KEY_5, AC_ARENA_SAVE);
  input.bindKey(RETRO_KEY_6, AC_ARENA_LOAD);

  ANIMATEDSPRITE_QUOTE_IDLE = sprites.loadAnimationH(SPRITESHEET, 1, 100, 0, 80, 16, 16);
  ANIMATEDSPRITE_QUOTE_WALK = sprites.loadAnimationH(SPRITESHEET, 4, 120, 0, 80, 16, 16);
}

RETRO_USER_START_API void Start()
{
  state = Retro_Scope_New(GameState);

  sprites.newAnimation(&state->player, ANIMATEDSPRITE_QUOTE_WALK, canvas.width() / 2, canvas.height() / 2);
  sprites.playAnimation(&state->player, true, true);

  state->velocity.x = 0;
  state->velocity.y = 0;

  timer.make(&TIMER);
  timer.start(&TIMER);

  audio.playMusic("origin.mod");

  //Retro_StartFromLibrary("LibGame.dll", LF_AsCopy);

  canvas.flags(0, CNF_Clear | CNF_Render, palette.black);
}

RETRO_USER_STEP_API void Step()
{


  if (input.released(AC_ACTION))
  {
    audio.playSound(SOUND_COIN, 128);
  }

  if (input.released(AC_MUSIC_ON))
  {
    audio.playMusic("origin.mod");
  }

  if (input.released(AC_MUSIC_OFF))
  {
    audio.stopMusic();
  }

  if (input.released(AC_ARENA_SAVE))
  {
    arena.save("arena.raw");
  }

  if (input.released(AC_ARENA_LOAD))
  {
    arena.load("arena.raw", true);
  }

  if (input.down(AC_UP))
    state->velocity.y -= 1;
  else if (input.down(AC_DOWN))
    state->velocity.y += 1;

  if (input.down(AC_LEFT))
    state->velocity.x--;
  else if (input.down(AC_RIGHT))
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
  else if (state->player.x + state->player.w > canvas.width())
  {
    state->player.x = canvas.width() - state->player.w;
    state->velocity.x = 0;
  }

  if (state->velocity.x != 0)
  {
    if (state->player.animationHandle == ANIMATEDSPRITE_QUOTE_IDLE)
    {
      sprites.setAnimation(&state->player, ANIMATEDSPRITE_QUOTE_WALK, true);
    }
  }
  else
  {
    if (state->player.animationHandle == ANIMATEDSPRITE_QUOTE_WALK)
    {
      sprites.setAnimation(&state->player, ANIMATEDSPRITE_QUOTE_IDLE, false);
    }
  }

  //canvas.use(0);
  //state->player.x -= 10;
  canvas.animate(&state->player, true);
  //state->player.x += 10;
  
  //canvas.use(1);
  //canvas.animate(&state->player, true);
  
  U32 ms = timer.ticks(&TIMER);
  canvas.printf(10, 10, &FONT_NEOSANS, palette.peach, "%i", ms);

  Retro_Debug(&FONT_NEOSANS);


}
