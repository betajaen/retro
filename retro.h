#ifndef RETRO_H
#define RETRO_H

#ifdef _WIN32
#define RETRO_WINDOWS
#endif

#ifdef __EMSCRIPTEN__
#define RETRO_BROWSER
#endif

#define Kilobytes(N) ((N) * 1024)
#define Megabytes(N) (Kilobytes(N) * 1024)

#ifndef RETRO_WINDOW_CAPTION
#define RETRO_WINDOW_CAPTION "Retro"
#endif

#ifndef RETRO_WINDOW_DEFAULT_WIDTH
#define RETRO_WINDOW_DEFAULT_WIDTH 640
#endif

#ifndef RETRO_WINDOW_DEFAULT_HEIGHT
#define RETRO_WINDOW_DEFAULT_HEIGHT 480
#endif

#ifndef RETRO_CANVAS_DEFAULT_WIDTH
#define RETRO_CANVAS_DEFAULT_WIDTH 320
#endif

#ifndef RETRO_CANVAS_DEFAULT_HEIGHT
#define RETRO_CANVAS_DEFAULT_HEIGHT 240
#endif

#ifndef RETRO_SOUND_DEFAULT_VOLUME
#define RETRO_SOUND_DEFAULT_VOLUME 1.0
#endif

#ifndef RETRO_FRAME_RATE
#define RETRO_FRAME_RATE 30
#endif

#ifndef RETRO_ARENA_SIZE
#define RETRO_ARENA_SIZE Kilobytes(1)
#endif

#ifndef RETRO_MAX_INPUT_ACTIONS
#define RETRO_MAX_INPUT_ACTIONS 32
#endif

#ifndef RETRO_MAX_INPUT_BINDINGS
#define RETRO_MAX_INPUT_BINDINGS 4
#endif

#ifndef RETRO_MAX_ANIMATED_SPRITE_FRAMES
#define RETRO_MAX_ANIMATED_SPRITE_FRAMES 8
#endif

#ifndef RETRO_CANVAS_COUNT
#define RETRO_CANVAS_COUNT 2
#endif

#ifndef RETRO_MAX_SOUND_OBJECTS
#define RETRO_MAX_SOUND_OBJECTS 16
#endif

#ifndef RETRO_AUDIO_FREQUENCY
#define RETRO_AUDIO_FREQUENCY 48000
#endif

#ifndef RETRO_AUDIO_CHANNELS
#define RETRO_AUDIO_CHANNELS 2
#endif

#ifndef RETRO_AUDIO_SAMPLES
#define RETRO_AUDIO_SAMPLES 16384
#endif 

#ifndef RETRO_NAMESPACES
#define RETRO_NAMESPACES 1
#endif

#if (RETRO_NAMESPACES == 1)

#ifndef RETRO_ARENA_NAMESPACE_NAME
#define RETRO_ARENA_NAMESPACE_NAME arena
#endif

#ifndef RETRO_SCOPE_NAMESPACE_NAME
#define RETRO_SCOPE_NAMESPACE_NAME scope
#endif

#ifndef RETRO_RESOURCES_NAMESPACE_NAME
#define RETRO_RESOURCES_NAMESPACE_NAME resources
#endif

#ifndef RETRO_CANVAS_NAMESPACE_NAME
#define RETRO_CANVAS_NAMESPACE_NAME canvas
#endif

#ifndef RETRO_PALETTE_NAMESPACE_NAME
#define RETRO_PALETTE_NAMESPACE_NAME palette
#endif

#ifndef RETRO_AUDIO_NAMESPACE_NAME
#define RETRO_AUDIO_NAMESPACE_NAME audio
#endif

#ifndef RETRO_INPUT_NAMESPACE_NAME
#define RETRO_INPUT_NAMESPACE_NAME input
#endif

#ifndef RETRO_FONT_NAMESPACE_NAME
#define RETRO_FONT_NAMESPACE_NAME font
#endif

#endif

#ifdef RETRO_AMERICAN
#define Color Colour
#endif

#define RETRO_UNUSED(X) (void)X
#define RETRO_ARRAY_COUNT(X) (sizeof(X) / sizeof((X)[0]))

#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef float    F32;
typedef double   F64;

#define Min(X, Y) (X < Y ? X : Y)
#define Max(X, Y) (X > Y ? X : Y)

typedef U8 SpriteHandle;
typedef U8 AnimationHandle;

typedef struct
{
  SDL_Texture*  texture;
  U8*    imageData;
  U16    w, h;
} Bitmap;

typedef struct
{
  S32 x, y;
} Point;

typedef struct
{
  S32 w, h;
} Size;

typedef struct
{
  S32 x, y, w, h;
} Rect;

typedef struct
{
  Bitmap*  bitmap;
  SDL_Rect rect;
  SpriteHandle spriteHandle;
} Sprite;

typedef struct
{
  Bitmap*         bitmap;
  U8              frameCount;
  U8              w, h;
  U16             frameLength;
  AnimationHandle animationHandle;
  Rect            frames[RETRO_MAX_ANIMATED_SPRITE_FRAMES];
} Animation;

typedef enum
{
  // Flip drawing X
  SOF_FlipX         = 1,
  // Flip drawing Y
  SOF_FlipY         = 2,
  // If AnimatedSpriteObject object then play animation
  SOF_Animation     = 4,
  // If AnimatedSpriteObject object then don't loop around the animation
  SOF_AnimationOnce = 8,
} SpriteObjectFlags;

typedef struct
{
  S32  x, y;
  S8   flags;
  U8   spriteHandle;
} SpriteObject;

typedef struct
{
  S32        x, y;
  U8         w, h;
  U8         frameNumber, flags;
  U8         animationHandle;
  U16        frameTime;
} AnimationObject;

typedef struct
{
  U8 r, g, b, a;
} Colour;

typedef struct
{
  Colour colours[256];
  U8     count, fallback, transparent;
} Palette;

typedef struct
{
  Bitmap bitmap;
  U8     height;
  U16    x[256];
  U8     widths[256];
} Font;

typedef struct
{
  U16     windowWidth;
  U16     windowHeight;
  U16     canvasWidth;
  U16     canvasHeight;
  F32     soundVolume;
  Palette palette;
} Settings;

typedef struct
{
  U32    start, paused;
  U8     flags;
} Timer;

typedef struct
{
  S32 length;
  U8* buffer;
  SDL_AudioSpec spec;
} Sound;

#define Point_Translate(P, X_VALUE, Y_VALUE) \
  (P)->x += X_VALUE; \
  (P)->y += Y_VALUE;

#define Point_Set(P, X_VALUE, Y_VALUE) \
  (P)->x = X_VALUE; \
  (P)->y = Y_VALUE;

#define Rect_Translate(R, X, Y) \
  (R)->left   += X; \
  (R)->top    += Y; \
  (R)->right  += X; \
  (R)->bottom += Y;

#define Rect_GetWidth(R) \
  ((R)->right - (R)->left)

#define Rect_GetHeight(R) \
  ((R)->right - (R)->left)

#define Colour_Equals(A, B) \
  (A.r == B.r && A.g == B.g && A.b == B.b)

Colour Colour_Make(U8 r, U8 g, U8 b);

Size Size_Make(U32 w, U32 h);




void Arena_Save(const char* filename);

void Arena_Load(const char* filename, bool loadMusic);

U8* Arena_SaveToMem(U32* size);

void Arena_LoadFromMem(U8* mem, bool loadMusic);

void Retro_Scope_Push(int name);

int  Retro_Scope_Name();

U8* Retro_Scope_Obtain(U32 size);

void Retro_Scope_Rewind();

void Retro_Scope_Pop();

#define Retro_Scope_New(T) ((T*) Retro_Scope_Obtain(sizeof(T)))


#if RETRO_NAMESPACES == 1
const struct RETRO_Scope
{
  void (*push)(int name);
  void (*pop)();
  int (*name)();
  U8* (*obtain)(U32 size);
  void (*rewind)();
}
RETRO_SCOPE_NAMESPACE_NAME
= {
  .push = Retro_Scope_Push,
  .pop  = Retro_Scope_Pop,
  .name = Retro_Scope_Name,
  .obtain = Retro_Scope_Obtain,
  .rewind = Retro_Scope_Rewind,
};
#elif RETRO_NAMESPACES == 0
#define Scope_Push(INT_name)                Retro_Scope_Push(INT_name)
#define Scope_Name()                        Retro_Scope_Name()
#define Scope_Obtain(U32_size)              Retro_Scope_Obtain(U32_size)
#define Scope_Rewind()                      Retro_Scope_Rewind()
#define Scope_Pop()                         Retro_Scope_Pop()
#endif



// Loads a palette from the colours given in a image file.
void  Retro_Resources_LoadPalette(const char* name);

// Loads a bitmap and matches the palette to the canvas palette best it can.
void  Retro_Resources_LoadBitmap(const char* name, Bitmap* outBitmap, U8 transparentIndex);

// Load a sound into Sound
void  Retro_Resources_LoadSound(const char* name, Sound* sound);

void  Retro_Resources_LoadFont(const char* name, Font* inFont, Colour markerColour, Colour transparentColour);

#if RETRO_NAMESPACES == 1
const struct RETRO_Resources
{
  void (*loadPalette)(const char* name);
  void (*loadBitmap)(const char* name, Bitmap* outBitmap, U8 transparentIndex);
  void (*loadSound)(const char* name, Sound* inSound);
  void (*loadFont)(const char* name, Font* inFont, Colour markerColour, Colour transparentColour);
}
RETRO_RESOURCES_NAMESPACE_NAME
= {
  .loadPalette = Retro_Resources_LoadPalette,
  .loadBitmap  = Retro_Resources_LoadBitmap,
  .loadSound   = Retro_Resources_LoadSound,
  .loadFont    = Retro_Resources_LoadFont
};
#elif RETRO_NAMESPACES == 0
#define  Resources_LoadPalette(CONST_CHAR_name)                                                             Retro_Resources_LoadPalette(CONST_CHAR_name)
#define  Resources_LoadBitmap(CONST_CHAR_name, BITMAP, U8_transparentIndex)                                 Retro_Resources_LoadBitmap(CONST_CHAR_name, BITMAP, U8_transparentIndex)
#define  Resources_LoadSound(CONST_CHAR_name, SOUND)                                                        Retro_Resources_LoadSound(CONST_CHAR_name, SOUND)
#define  Resources_Font_Load(CONST_CHAR_name, FONT_IN_font, COLOUR_markerColour, COLOUR_transparentColour)  Retro_Resources_LoadFont(CONST_CHAR_name, FONT_IN_font, COLOUR_markerColour, COLOUR_transparentColour)

#endif



void  Sprite_Make(Sprite* inSprite, Bitmap* bitmap, U32 x, U32 y, U32 w, U32 h);

Sprite* SpriteHandle_Get(SpriteHandle id);

void  Animation_LoadHorizontal(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

void  Animation_LoadVertical(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

Animation* AnimationHandle_Get(AnimationHandle id);






void  Retro_Canvas_Use(U8 canvasIndex);

void  Retro_Canvas_Clear();

typedef enum
{
  // Clear each frame
  CNF_Clear      = 1,
  // Apply blending to the canvas underneath
  CNF_Blend      = 2,
  // Render this canvas
  CNF_Render     = 4,
} CanvasFlags;

void  Retro_Canvas_Flags(U8 id, U8 flags, U8 clearColour);

typedef enum
{
  CC_None     = SDL_FLIP_NONE,
  CC_FlipHorz = SDL_FLIP_HORIZONTAL,
  CC_FlipVert = SDL_FLIP_VERTICAL,
  CC_FlipDiag = CC_FlipHorz | CC_FlipVert,
  CC_Mask     = CC_FlipHorz | CC_FlipVert
} Retro_CanvasCopyFlags;

typedef enum
{
  // 1
  FP_Normal,
  //  alpha + (k * beta)
  FP_WaveH,
  //  alpha + (k * beta)
  FP_WaveV,
  //  w *= alpha, h *= beta, x -= w/2, y -= h/2
  FP_Scale
} Retro_CanvasPresentation;

void  Retro_Canvas_Presentation(Retro_CanvasPresentation presentation, float alpha, float beta);

void  Retro_Canvas_Copy(Bitmap* bitmap, Rect* dstRectangle, Rect* srcRectangle, U8 copyFlags);

void  Retro_Canvas_Copy2(Bitmap* bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);

void  Retro_Canvas_Sprite(SpriteObject* spriteObject);

void  Retro_Canvas_Sprite2(SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);

void  Retro_Canvas_Animate(AnimationObject* animationObject, bool updateTiming);

void  Retro_Canvas_Animate2(AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);

void  Retro_Canvas_DrawPalette(S32 y);

void  Retro_Canvas_DrawBox(U8 colour, Rect rect);

void  Retro_Canvas_DrawRectangle(U8 colour, Rect rect);

void  Retro_Canvas_Print(S32 x, S32 y, Font* font, U8 colour, const char* str);

void  Retro_Canvas_Printf(S32 x, S32 y, Font* font, U8 colour, const char* fmt, ...);

#if RETRO_NAMESPACES == 1
const struct RETRO_Canvas
{
  Size size;
  U32  count;
  
  void (*use)(U8 canvasIndex);
  void (*flags)(U8 id, U8 flags, U8 clearColour);
  void (*clear)();
  void (*copy)(Bitmap* bitmap, Rect* dstRectangle, Rect* srcRectangle, U8 copyFlags);
  void (*copy2)(Bitmap* bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);
  void (*sprite)(SpriteObject* spriteObject);
  void (*sprite2)(SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);
  void (*animate)(AnimationObject* animationObject, bool updateTiming);
  void (*animate2)(AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);
  void (*drawPalette)(S32 y);
  void (*drawBox)(U8 colour, Rect rect);
  void (*drawRectangle)(U8 colour, Rect rect);
  void (*print)(S32 x, S32 y, Font* font, U8 colour, const char* str);
  void (*printf)(S32 x, S32 y, Font* font, U8 colour, const char* fmt, ...);
}
RETRO_CANVAS_NAMESPACE_NAME
= {
  .size          = { RETRO_CANVAS_DEFAULT_WIDTH, RETRO_CANVAS_DEFAULT_HEIGHT },
  .count         = RETRO_CANVAS_COUNT,
  .use           = Retro_Canvas_Use,
  .flags         = Retro_Canvas_Flags,
  .clear         = Retro_Canvas_Clear,
  .copy          = Retro_Canvas_Copy,
  .copy2         = Retro_Canvas_Copy2,
  .sprite        = Retro_Canvas_Sprite,
  .sprite2       = Retro_Canvas_Sprite2,
  .animate       = Retro_Canvas_Animate,
  .animate2      = Retro_Canvas_Animate2,
  .drawBox       = Retro_Canvas_DrawBox,
  .drawRectangle = Retro_Canvas_DrawRectangle,
  .print         = Retro_Canvas_Print,
  .printf        = Retro_Canvas_Printf
};
#elif RETRO_NAMESPACES == 0
#define Canvas_Use(U8_canvasIndex)                                                                Retro_Canvas_Use(U8_canvasIndex)
#define Canvas_Clear()                                                                            Retro_Canvas_Clear()
#define Canvas_Flags(U8_id, U8_flags, U8_clearColour)                                             Retro_Canvas_Flags(U8_id, U8_flags, U8_clearColour)
#define Canvas_Presentation(CANVASPRESENTATION_presentation, FLOAT_alpha, FLOAT_beta)             Retro_Canvas_Presentation(CANVASPRESENTATION_presentation, FLOAT_alpha, FLOAT_beta)
#define Canvas_Copy(BITMAP, RECT_dstRect, RECT_srcRect, U8_copyFlags)                             Retro_Canvas_Copy(BITMAP, RECT_dstRect, RECT_srcRect, U8_copyFlags)
#define Canvas_Copy2(BITMAP, S32_dstX, S32_dstY, S32_srcX, S32_srcY, S32_w, S32_h, U8_copyFlags)  Retro_Canvas_Copy2(BITMAP, S32_dstX, S32_dstY, S32_srcX, S32_srcY, S32_w, S32_h, U8_copyFlags)
#define Canvas_Sprite(SPRITEOBJECT)                                                               Retro_Canvas_Sprite(SPRITEOBJECT)
#define Canvas_Sprite2(SPRITEHANDLE, S32_x, S32_y, U8_flipFlags)                                  Retro_Canvas_Sprite2(SPRITEHANDLE, S32_x, S32_y, U8_flipFlags)
#define Canvas_Animate(ANIMATIONOBJECT, BOOL_updateTiming)                                        Retro_Canvas_Animate(ANIMATIONOBJECT, BOOL_updateTiming)
#define Canvas_Animate2(ANIMATIONHANDLE, S32_x, S32_y, U8_frame, U8_copyFlags)                    Retro_Canvas_Animate2(ANIMATIONHANDLE, S32_x, S32_y, U8_frame, U8_copyFlags)
#define Canvas_DrawPalette(S32_y)                                                                 Retro_Canvas_DrawPalette(S32_y)
#define Canvas_DrawBox(U8_colour, RECT_rect)                                                      Retro_Canvas_DrawBox(U8_colour, RECT_rect)
#define Canvas_DrawRectangle(U8_colour, RECT_rect)                                                Retro_Canvas_DrawRectangle(U8_colour, RECT_rect)
#define Canvas_Print(S32_x, S32_y, FONT, U8_colour, CONST_CHAR_str)                               Retro_Canvas_Print(S32_x, S32_y, FONT, U8_colour, CONST_CHAR_str)
#define Canvas_Printf(S32_x, S32_y, FONT_font, U8_colour, CONST_CHAR_fmt, ...)                    Retro_Canvas_Printf(S32_x, S32_y, FONT_font, U8_colour, CONST_CHAR_fmt, __VA_ARGS__)
#endif


void  AnimatedSpriteObject_Make(AnimationObject* inAnimatedSpriteObject, Animation* animation, S32 x, S32 y);

void  AnimatedSpriteObject_PlayAnimation(AnimationObject* animatedSpriteObject, bool playing, bool loop);

void  AnimatedSpriteObject_SwitchAnimation(AnimationObject* animatedSpriteObject, Animation* newAnimation, bool animate);

void  Retro_Audio_PlaySound(Sound* sound, U8 volume);

void  Retro_Audio_ClearSounds();

void  Retro_Audio_PlayMusic(const char* name);

void  Retro_Audio_StopMusic();

#if RETRO_NAMESPACES == 1
  const struct RETRO_Audio
  {
    void (*playSound)(Sound* inSound, U8 volume);
    void (*clearSounds)();
    void (*playMusic)(const char* name);
    void (*stopMusic)();
  }
  RETRO_AUDIO_NAMESPACE_NAME
  = {
    .playSound   = Retro_Audio_PlaySound,
    .clearSounds = Retro_Audio_ClearSounds,
    .playMusic   = Retro_Audio_PlayMusic,
    .stopMusic   = Retro_Audio_StopMusic
  };
#elif RETRO_NAMESPACES == 0
#define    Audio_PlaySound(SOUND, U8_volume)              Retro_Audio_PlaySound(SOUND, U8_volume)
#define    Audio_ClearSounds()                            Retro_Audio_ClearSounds()
#define    Audio_PlayMusic(CONST_CHAR_name)               Retro_Audio_PlayMusic(CONST_CHAR_name)
#define    Audio_StopMusic()                              Retro_Audio_StopMusic()
#endif

void  Retro_Palette_Add(Colour colour);

void  Retro_Palette_AddRGB(U32 rgb);

U8    Retro_Palette_Index(Colour colour);

bool  Retro_Palette_Has(Colour colour);

Colour Retro_Palette_Get(U8 index);

#if RETRO_NAMESPACES == 1
  const struct RETRO_Palette {
    void (*add)(Colour colour);
    void (*addRGB)(U32 argb);
    U8   (*index)(Colour colour);
    bool (*has)(Colour colour);
    Colour (*get)(U8 index);
  }
  RETRO_PALETTE_NAMESPACE_NAME
  = {
    .add     = Retro_Palette_Add,
    .addRGB  = Retro_Palette_AddRGB,
    .index   = Retro_Palette_Index,
    .has     = Retro_Palette_Has,
    .get     = Retro_Palette_Get
  };
#elif RETRO_NAMESPACES == 0
#   define  Palette_Add(COLOUR_colour)                Retro_Palette_Add(COLOUR_colour)
#   define  Palette_AddRGB(U32_argb)                  Retro_Palette_AddARGB(U32_argb)
#   define  Palette_Index(COLOUR_colour)              Retro_Palette_Index(COLOUR_colour)
#   define  Palette_Has(COLOUR_colour)                Retro_Palette_Has(COLOUR_colour)
#   define  Palette_Get(U8_index)                     Retro_Palette_Get(U8_index)
#endif

int   Retro_Input_TextInput(char* str, U32 capacity);

void  Retro_Input_BindKey(int sdl_scancode, int action);

void  Retro_Input_BindAxis(int axis, int action);

bool  Retro_Input_Down(int action);

bool  Retro_Input_Released(int action);

bool  Retro_Input_Pressed(int action);

S16   Retro_Input_Axis(int action);

S16   Retro_Input_DeltaAxis(int action);

#if RETRO_NAMESPACES == 1
  const struct RETRO_Input {
    int(*textInput)(char* str, U32 capacity);
    void(*bindKey)(int sdl_scancode, int action);
    void(*bindAxis)(int axis, int action);
    bool(*down)(int action);
    bool(*released)(int action);
    bool(*pressed)(int action);
    S16(*axis)(int action);
    S16(*deltaAxis)(int action);
  }
  RETRO_INPUT_NAMESPACE_NAME
  = {
    .textInput = Retro_Input_TextInput,
    .bindKey   = Retro_Input_BindKey,
    .bindAxis  = Retro_Input_BindAxis,
    .down      = Retro_Input_Down,
    .released  = Retro_Input_Released,
    .pressed   = Retro_Input_Pressed,
    .axis      = Retro_Input_Axis,
    .deltaAxis = Retro_Input_DeltaAxis
  };
#elif RETRO_NAMESPACES == 0
#   define Input_TextInput(CHAR_str, U32_capacity)         Retro_Input_TextInput(CHAR_str, U32_capacity)
#   define Input_BindKey(INT_sdlscancode, INT_action)      Retro_Input_BindKey(INT_sdlscancode, INT_action)
#   define Input_BindAxis(INT_axis, INT_action)            Retro_Input_BindAxis(INT_axis, INT_action)
#   define Input_Down(INT_action)                          Retro_Input_Down(INT_action)
#   define Input_Released(INT_action)                      Retro_Input_Released(INT_action)
#   define Input_Pressed(INT_action)                       Retro_Input_Pressed(INT_action)
#   define Input_NowAxis(INT_action)                       Retro_Input_NowAxis(INT_action)
#   define Input_DeltaAxis(INT_action)                     Retro_Input_DeltaAxis(INT_action)
#endif

void  Retro_Timer_Make(Timer* timer);

void  Retro_Timer_Start(Timer* timer);

void  Retro_Timer_Stop(Timer* timer);

void  Retro_Timer_Pause(Timer* timer);

void  Retro_Timer_Unpause(Timer* timer);

U32   Retro_Timer_Ticks(Timer* timer);

bool  Retro_Timer_Started(Timer* timer);

bool  Retro_Timer_Paused(Timer* timer);

#if RETRO_NAMESPACES == 1
  const struct RETRO_Timer {
    void (*make)(Timer* timer);
    void (*start)(Timer* timer);
    void (*stop)(Timer* timer);
    void (*pause)(Timer* timer);
    void (*unpause)(Timer* timer);
    U32  (*ticks)(Timer* timer);
    bool (*started)(Timer* timer);
    bool (*paused)(Timer* timer);
  } 
  #ifdef RETRO_TIMER_IS
    RETRO_TIMER_IS
  #else
    timer
  #endif
  = {
    .make    = Retro_Timer_Make,
    .start   = Retro_Timer_Start,
    .stop    = Retro_Timer_Stop,
    .pause   = Retro_Timer_Pause,
    .unpause = Retro_Timer_Unpause,
    .ticks   = Retro_Timer_Ticks,
    .started = Retro_Timer_Started,
    .paused  = Retro_Timer_Paused,
  };
#elif RETRO_NAMESPACES == 0
#   define Timer_Make(TIMER)      Retro_TimerMake(TIMER)
#   define Timer_Start(TIMER)     Retro_Timer_Start(TIMER)
#   define Timer_Stop(TIMER)      Retro_Timer_Stop(TIMER)
#   define Timer_Pause(TIMER)     Retro_Timer_Pause(TIMER)
#   define Timer_Unpause(TIMER)   Retro_Timer_Unpause(TIMER)
#   define Timer_Ticks(TIMER)     Retro_Timer_Ticks(TIMER)
#   define Timer_Started(TIMER)   Retro_Timer_Started(TIMER)
#   define Timer_Paused(TIMER)    Retro_Timer_Paused(TIMER)
#endif

void  Init(Settings* s);

void  Start();

void  Step();


#endif
