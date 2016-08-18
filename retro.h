#ifndef RETRO_H
#define RETRO_H

#ifdef _WIN32
#define RETRO_WINDOWS
#endif

#ifdef __EMSCRIPTEN__
#define RETRO_BROWSER
#endif

#define RETRO_KILOBYTES(N) \
  ((N) * 1024)

#define RETRO_MEGABYTES(N) \
  (RETRO_KILOBYTES(N) * 1024)

#ifndef RETRO_DEFAULT_WINDOW_CAPTION
#define RETRO_DEFAULT_WINDOW_CAPTION "Retro"
#endif

#ifndef RETRO_DEFAULT_WINDOW_WIDTH
#define RETRO_DEFAULT_WINDOW_WIDTH 640
#endif

#ifndef RETRO_DEFAULT_WINDOW_HEIGHT
#define RETRO_DEFAULT_WINDOW_HEIGHT 480
#endif

#ifndef RETRO_DEFAULT_CANVAS_WIDTH
#define RETRO_DEFAULT_CANVAS_WIDTH 320
#endif

#ifndef RETRO_DEFAULT_CANVAS_HEIGHT
#define RETRO_DEFAULT_CANVAS_HEIGHT 240
#endif

#ifndef RETRO_DEFAULT_SOUND_DEFAULT_VOLUME
#define RETRO_DEFAULT_SOUND_DEFAULT_VOLUME 1.0
#endif

#ifndef RETRO_DEFAULT_FRAME_RATE
#define RETRO_DEFAULT_FRAME_RATE 30
#endif

#ifndef RETRO_DEFAULT_ARENA_SIZE
#define RETRO_DEFAULT_ARENA_SIZE RETRO_KILOBYTES(1)
#endif

#ifndef RETRO_DEFAULT_MAX_INPUT_ACTIONS
#define RETRO_DEFAULT_MAX_INPUT_ACTIONS 32
#endif

#ifndef RETRO_DEFAULT_MAX_BITMAPS
#define RETRO_DEFAULT_MAX_BITMAPS 16
#endif

#ifndef RETRO_DEFAULT_MAX_SOUNDS
#define RETRO_DEFAULT_MAX_SOUNDS 64
#endif
#ifndef RETRO_DEFAULT_MAX_ANIMATIONS
#define RETRO_DEFAULT_MAX_ANIMATIONS 256
#endif

#ifndef RETRO_DEFAULT_MAX_SPRITES
#define RETRO_DEFAULT_MAX_SPRITES 256
#endif

#ifndef RETRO_DEFAULT_CANVAS_COUNT
#define RETRO_DEFAULT_CANVAS_COUNT 2
#endif

#ifndef RETRO_DEFAULT_MAX_SOUND_OBJECTS
#define RETRO_DEFAULT_MAX_SOUND_OBJECTS 16
#endif

#ifndef RETRO_DEFAULT_AUDIO_FREQUENCY
#define RETRO_DEFAULT_AUDIO_FREQUENCY 48000
#endif

#ifndef RETRO_DEFAULT_AUDIO_CHANNELS
#define RETRO_DEFAULT_AUDIO_CHANNELS 2
#endif

#ifndef RETRO_DEFAULT_AUDIO_SAMPLES
#define RETRO_DEFAULT_AUDIO_SAMPLES 16384
#endif 

#ifndef RETRO_NAMESPACES
#define RETRO_NAMESPACES 1
#endif

#ifndef RETRO_API
#  if defined(RETRO_WINDOWS)
#     if defined(RETRO_COMPILING_AS_LIBRARY)
//        LibRetro.dll
#         define RETRO_API __declspec(dllexport)
#     elif defined(RETRO_USING_AS_LIBRARY)
//        LibGame.dll or Game.exe using LibRetro.dll
#         define RETRO_API __declspec(dllimport)
#     else
//        Game.exe with Retro.c #included
#         define RETRO_API
#     endif
#  else
#         define RETRO_API extern
#  endif
#endif

#if !defined(RETRO_USER_INIT_API)
#  if defined(RETRO_WINDOWS)
#     if defined(RETRO_COMPILING_AS_LIBRARY)
//        LibRetro.dll
#         define RETRO_USER_INIT_API 
#         define RETRO_USER_START_API 
#         define RETRO_USER_STEP_API 
#         define RETRO_USER_SETTINGS_API
#     elif defined(RETRO_USING_AS_LIBRARY)
//        LibGame.dll or Game.exe using LibRetro.dll
#         define RETRO_USER_INIT_API      __declspec(dllexport)
#         define RETRO_USER_START_API     __declspec(dllexport)
#         define RETRO_USER_STEP_API      __declspec(dllexport)
#         define RETRO_USER_SETTINGS_API  extern
#     else
//        Game.exe with Retro.c #included
#         define RETRO_USER_INIT_API
#         define RETRO_USER_START_API
#         define RETRO_USER_STEP_API
#         define RETRO_USER_SETTINGS_API
#     endif
#  else
#         define RETRO_API extern
#  endif
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

#ifndef RETRO_SPRITES_NAMESPACE_NAME
#define RETRO_SPRITES_NAMESPACE_NAME sprites
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

#ifndef RETRO_TIMER_NAMESPACE_NAME
#define RETRO_TIMER_NAMESPACE_NAME timer
#endif

#ifndef RETRO_DEFAULT_PALETTE
#define RETRO_DEFAULT_PALETTE 'DB16'
#endif
#endif

#ifdef RETRO_AMERICAN
#define Color Colour
#endif

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef float    F32;
typedef double   F64;

typedef U16 SpriteHandle;
typedef U16 AnimationHandle;
typedef U16 BitmapHandle;
typedef U16 SoundHandle;

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
  U8 r, g, b, a;
} Colour;

typedef struct
{
  Colour colours[256];
  U16    count;
  U8     fallback, transparent;
} Palette;

typedef struct
{
  BitmapHandle bitmap;
  U8           height;
  U16          x[256];
  U8           widths[256];
} Font;

typedef struct
{
  U32    start, paused;
  U8     flags;
} Timer;

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

#define Retro_Min(X, Y) \
  (X < Y ? X : Y)

#define Retro_Max(X, Y) \
  (X > Y ? X : Y)

#define Retro_Point_Translate(P, X_VALUE, Y_VALUE) \
  (P)->x += X_VALUE; \
  (P)->y += Y_VALUE;

#define Retro_Point_Set(P, X_VALUE, Y_VALUE) \
  (P)->x = X_VALUE; \
  (P)->y = Y_VALUE;

#define Retro_Rect_Translate(R, X, Y) \
  (R)->x += X; \
  (R)->y += Y;

#define Retro_Colour_Equals(A, B) \
  (A.r == B.r && A.g == B.g && A.b == B.b)

#define RETRO_UNUSED(X) \
  ((void) X)

#define RETRO_ARRAY_COUNT(X) \
  (sizeof(X) / sizeof((X)[0]))

RETRO_API U8     Hex_Char(char str);
RETRO_API U8     Hex_U8(const char* str);
RETRO_API S8     Hex_S8(const char* str);
RETRO_API U16    Hex_U16(const char* str);
RETRO_API S16    Hex_S16(const char* str);
RETRO_API U32    Hex_U32(const char* str);
RETRO_API S32    Hex_S32(const char* str);
RETRO_API Colour Hex_Colour(const char* str);
RETRO_API Colour Colour_Make(U8 r, U8 g, U8 b);
RETRO_API Point  Point_Make(S32 x, S32 y);
RETRO_API Size   Size_Make(U32 w, U32 h);
RETRO_API Rect   Rect_Make(S32 x, S32 y, S32 w, S32 h);

#if defined(RETRO_SHORTHAND)
#   define _RGB(STR)           Hex_Colour(STR)
#   define _POINT(X, Y)        Point_Make(X, Y)
#   define _RECT(X, Y, W, H)   Rect_Make(X, Y, W, H)
#   define _SIZE(W, H)         Size_Make(W, H)
#   define _KILOBYTES(BYTES)   RETRO_KILOBYTES(BYTES)
#   define _MEGABYTES(BYTES)   RETRO_MEGABYTES(BYTES)
#endif

typedef struct
{
  const char* caption;
  U32         windowWidth, windowHeight;
  U32         canvasWidth, canvasHeight;
  Rect        viewport;
  int         defaultPalette;
  float       soundVolume;
  U32         frameRate;
  U32         arenaSize;
  U16         maxInputActions;
  U16         maxBitmaps;
  U16         maxSounds;
  U16         maxAnimations;
  U16         maxSprites;
  U8          canvasCount;
  U8          maxSoundObjects;
  U16         audioFrequency;
  U8          audioChannels;
  U32         audioSamples;
} Retro_Settings;

const Retro_Settings Retro_Default_Settings = {
  RETRO_DEFAULT_WINDOW_CAPTION,
  RETRO_DEFAULT_WINDOW_WIDTH,
  RETRO_DEFAULT_WINDOW_HEIGHT,
  RETRO_DEFAULT_CANVAS_WIDTH,
  RETRO_DEFAULT_CANVAS_HEIGHT,
  { 0, 0, 0, 0 },
  #ifdef RETRO_DEFAULT_PALETTE
  RETRO_DEFAULT_PALETTE,
  #else
  0,
  #endif
  RETRO_DEFAULT_SOUND_DEFAULT_VOLUME,
  RETRO_DEFAULT_FRAME_RATE,
  RETRO_DEFAULT_ARENA_SIZE,
  RETRO_DEFAULT_MAX_INPUT_ACTIONS,
  RETRO_DEFAULT_MAX_BITMAPS,
  RETRO_DEFAULT_MAX_SOUNDS,
  RETRO_DEFAULT_MAX_ANIMATIONS,
  RETRO_DEFAULT_MAX_SPRITES,
  RETRO_DEFAULT_CANVAS_COUNT,
  RETRO_DEFAULT_MAX_SOUND_OBJECTS,
  RETRO_DEFAULT_AUDIO_FREQUENCY,
  RETRO_DEFAULT_AUDIO_CHANNELS,
  RETRO_DEFAULT_AUDIO_SAMPLES
};

// 
RETRO_API int  Retro_Start(Retro_Settings* settings, void(*initFunction)(), void(*startFunction)(), void(*stepFunction)());

// 
RETRO_API void Retro_Shutdown();

typedef enum
{
  LF_None   = 0,
  LF_AsCopy = (1 << 0),
} RetroLibraryFlags;

RETRO_API int Retro_StartFromLibrary(const char* libraryPath, U8 libraryFlags);

#if !(defined(RETRO_COMPILING_AS_LIBRARY) || defined(RETRO_USING_RETRO_AS_LIBRARY) )

RETRO_USER_INIT_API  void Init();
RETRO_USER_START_API void Start();
RETRO_USER_STEP_API  void Step();

RETRO_API void Retro_Scope_Push(int name);

#ifndef RETRO_NO_MAIN
#define RETRO_NO_MAIN
int main(int argc, char *argv[])
{
  Retro_Settings settings = Retro_Default_Settings;
  Retro_Start(&settings, Init, Start, Step);
  Retro_Shutdown();
  return 0;
}
#endif

#endif







RETRO_API void Retro_Arena_LoadFromMemory(U8* mem, bool loadMusic);

RETRO_API void Retro_Arena_Load(const char* filename, bool loadMusic);

RETRO_API U8* Retro_Arena_SaveToMemory(U32* outSize);

RETRO_API void Retro_Arena_Save(const char* filename);

#if RETRO_NAMESPACES == 1
const struct RETRO_Arena
{
  U32  size;
  void (*loadFromMemory)(U8* mem, bool loadMusic);
  void (*load)(const char* filename, bool loadMusic);
  U8*  (*saveToMemory)(U32* outSize);
  void (*save)(const char* filename);
}
RETRO_ARENA_NAMESPACE_NAME
= {
  .size          = RETRO_DEFAULT_ARENA_SIZE,
  .loadFromMemory = Retro_Arena_LoadFromMemory,
  .load           = Retro_Arena_Load,
  .saveToMemory   = Retro_Arena_SaveToMemory,
  .save           = Retro_Arena_Save
};
#endif

#if defined(RETRO_SHORTHAND)
#   define Arena_LoadFromMemory       Retro_Arena_LoadFromMemory
#   define Arena_Load                 Retro_Arena_Load
#   define Arena_SaveToMemory         Retro_Arena_SaveToMemory
#   define Arena_Save                 Retro_Arena_Save
#endif








RETRO_API void Retro_Scope_Push(int name);

RETRO_API int  Retro_Scope_Name();

RETRO_API U8* Retro_Scope_Obtain(U32 size);

RETRO_API void Retro_Scope_Rewind();

RETRO_API void Retro_Scope_Pop();

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
  Retro_Scope_Push,
  Retro_Scope_Pop,
  Retro_Scope_Name,
  Retro_Scope_Obtain,
  Retro_Scope_Rewind,
};
#endif

#if defined(RETRO_SHORTHAND)
#   define Scope_Push              Retro_Scope_Push
#   define Scope_Name              Retro_Scope_Name
#   define Scope_Obtain            Retro_Scope_Obtain
#   define Scope_Rewind            Retro_Scope_Rewind
#   define Scope_Pop               Retro_Scope_Pop
#endif










// Loads a palette from the colours given in a image file.
RETRO_API void  Retro_Resources_LoadPalette(const char* name);

// Loads a bitmap and matches the palette to the canvas palette best it can.
RETRO_API BitmapHandle  Retro_Resources_LoadBitmap(const char* name, U8 transparentIndex);

// Load a sound into Sound
RETRO_API SoundHandle Retro_Resources_LoadSound(const char* name);

// Load a font using the Grafx2 format.
RETRO_API void  Retro_Resources_LoadFont(const char* name, Font* inFont, Colour markerColour, Colour transparentColour);

// Load a font that has a fixed width and arranged horizontally - like GrafX2 (but without the marker line)
RETRO_API void  Retro_Resources_LoadFontFixed(const char* name, Font* inFont, U8 w, Colour transparentColour);

#if RETRO_NAMESPACES == 1
const struct RETRO_Resources
{
  void         (*loadPalette)(const char* name);
  BitmapHandle (*loadBitmap)(const char* name, U8 transparentIndex);
  SoundHandle  (*loadSound)(const char* name);
  void         (*loadFont)(const char* name, Font* inFont, Colour markerColour, Colour transparentColour);
  void         (*loadFontFixed)(const char* name, Font* inFont, U8 w, Colour transparentColour);
}
RETRO_RESOURCES_NAMESPACE_NAME
= {
  Retro_Resources_LoadPalette,
  Retro_Resources_LoadBitmap,
  Retro_Resources_LoadSound,
  Retro_Resources_LoadFont,
  Retro_Resources_LoadFontFixed
};
#endif

#if defined(RETRO_SHORTHAND)
#  define  Resources_LoadPalette      Retro_Resources_LoadPalette
#  define  Resources_LoadBitmap       Retro_Resources_LoadBitmap
#  define  Resources_LoadSound        Retro_Resources_LoadSound
#  define  Resources_Font_Load        Retro_Resources_LoadFont
#endif






RETRO_API SpriteHandle Retro_Sprites_LoadSprite(BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h);

RETRO_API AnimationHandle Retro_Sprites_LoadAnimationH(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

RETRO_API AnimationHandle Retro_Sprites_LoadAnimationV(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

RETRO_API void  Retro_Sprites_NewAnimationObject(AnimationObject* inAnimatedSpriteObject, AnimationHandle animation, S32 x, S32 y);

RETRO_API void  Retro_Sprites_PlayAnimationObject(AnimationObject* animatedSpriteObject, bool playing, bool loop);

RETRO_API void  Retro_Sprites_SetAnimationObject(AnimationObject* animatedSpriteObject, AnimationHandle newAnimation, bool animate);

#if RETRO_NAMESPACES == 1
const struct RETRO_Sprites
{
  SpriteHandle (*loadSprite)(BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h);
  AnimationHandle (*loadAnimationH)(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);
  AnimationHandle (*loadAnimationV)(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);
  void (*newAnimation)(AnimationObject* inAnimatedSpriteObject, AnimationHandle animation, S32 x, S32 y);
  void (*playAnimation)(AnimationObject* animatedSpriteObject, bool playing, bool loop);
  void (*setAnimation)(AnimationObject* animatedSpriteObject, AnimationHandle newAnimation, bool animate);
}
RETRO_SPRITES_NAMESPACE_NAME
= {
  Retro_Sprites_LoadSprite,
  Retro_Sprites_LoadAnimationH,
  Retro_Sprites_LoadAnimationV,
  Retro_Sprites_NewAnimationObject,
  Retro_Sprites_PlayAnimationObject,
  Retro_Sprites_SetAnimationObject
};
#endif

#if defined(RETRO_SHORTHAND)
#   define Sprites_LoadSprite              Retro_Sprites_LoadSprite
#   define Sprites_LoadAnimationH          Retro_Sprites_LoadAnimationH
#   define Sprites_LoadAnimationV          Retro_Sprites_LoadAnimationV
#   define Sprites_MakeAnimationObject     Retro_Sprites_MakeAnimationObject
#   define Sprites_PlayAnimationObject     Retro_Sprites_PlayAnimationObject
#   define Sprites_SetAnimationObject      Retro_Sprites_SetAnimationObject
#endif








typedef enum
{
  // Clear each frame
  CNF_Clear      = 1,
  // Apply blending to the canvas underneath
  CNF_Blend      = 2,
  // Render this canvas
  CNF_Render     = 4,
} CanvasFlags;

typedef enum
{
  CC_None     = 0,
  CC_FlipHorz = 1,
  CC_FlipVert = 2,
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

RETRO_API S32   Retro_Canvas_Width();

RETRO_API S32   Retro_Canvas_Height();

RETRO_API void  Retro_Canvas_Use(U8 canvasIndex);

RETRO_API void  Retro_Canvas_Clear();

RETRO_API void  Retro_Canvas_Flags(U8 id, U8 flags, U8 clearColour);

RETRO_API void  Retro_Canvas_Presentation(Retro_CanvasPresentation presentation, float alpha, float beta);

RETRO_API void  Retro_Canvas_Copy(BitmapHandle bitmap, Rect* dstRectangle, Rect* srcRectangle, U8 copyFlags);

RETRO_API void  Retro_Canvas_Copy2(BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);

RETRO_API void  Retro_Canvas_Sprite(SpriteObject* spriteObject);

RETRO_API void  Retro_Canvas_Animate(AnimationObject* animationObject, bool updateTiming);

RETRO_API void  Retro_Canvas_DrawPalette(S32 y);

RETRO_API void  Retro_Canvas_DrawPalette2(S32 y, U8 from, U8 to);

RETRO_API void  Retro_Canvas_DrawBox(U8 colour, Rect rect);

RETRO_API void  Retro_Canvas_DrawRectangle(U8 colour, Rect rect);

RETRO_API void  Retro_Canvas_DrawSprite(SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);

RETRO_API void  Retro_Canvas_DrawAnimation(AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);

RETRO_API void  Retro_Canvas_Print(S32 x, S32 y, Font* font, U8 colour, const char* str);

RETRO_API void  Retro_Canvas_Printf(S32 x, S32 y, Font* font, U8 colour, const char* fmt, ...);

RETRO_API void  Retro_Canvas_MonoPrint(S32 x, S32 y, S32 w, Font* font, U8 colour, const char* str);

#if RETRO_NAMESPACES == 1
const struct RETRO_Canvas
{
  S32  (*width)();
  S32  (*height)();
  void (*use)(U8 canvasIndex);
  void (*flags)(U8 id, U8 flags, U8 clearColour);
  void (*clear)();
  void (*copy)(BitmapHandle bitmap, Rect* dstRectangle, Rect* srcRectangle, U8 copyFlags);
  void (*copy2)(BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);
  void (*sprite)(SpriteObject* spriteObject);
  void (*animate)(AnimationObject* animationObject, bool updateTiming);
  void (*drawPalette)(S32 y);
  void (*drawPalette2)(S32 y, U8 from, U8 to);
  void (*drawBox)(U8 colour, Rect rect);
  void (*drawRectangle)(U8 colour, Rect rect);
  void (*drawSprite)(SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);
  void (*drawAnimation)(AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);
  void (*print)(S32 x, S32 y, Font* font, U8 colour, const char* str);
  void (*printf)(S32 x, S32 y, Font* font, U8 colour, const char* fmt, ...);
  void (*monoPrint)(S32 x, S32 y, S32 w, Font* font, U8 colour, const char* str);
}
RETRO_CANVAS_NAMESPACE_NAME
= {
  Retro_Canvas_Width,
  Retro_Canvas_Height,
  Retro_Canvas_Use,
  Retro_Canvas_Flags,
  Retro_Canvas_Clear,
  Retro_Canvas_Copy,
  Retro_Canvas_Copy2,
  Retro_Canvas_Sprite,
  Retro_Canvas_Animate,
  Retro_Canvas_DrawPalette,
  Retro_Canvas_DrawPalette2,
  Retro_Canvas_DrawBox,
  Retro_Canvas_DrawRectangle,
  Retro_Canvas_DrawSprite,
  Retro_Canvas_DrawAnimation,
  Retro_Canvas_Print,
  Retro_Canvas_Printf,
  Retro_Canvas_MonoPrint
};
#endif

#if defined(RETRO_SHORTHAND)
#   define Canvas_Width                                    Retro_Canvas_Width
#   define Canvas_Height                                   Retro_Canvas_Height
#   define Canvas_Use                                      Retro_Canvas_Use
#   define Canvas_Flags                                    Retro_Canvas_Flags
#   define Canvas_Clear                                    Retro_Canvas_Clear
#   define Canvas_Copy                                     Retro_Canvas_Copy
#   define Canvas_Copy2                                    Retro_Canvas_Copy2
#   define Canvas_Sprite                                   Retro_Canvas_Sprite
#   define Canvas_Sprite2                                  Retro_Canvas_DrawSprite
#   define Canvas_Animate                                  Retro_Canvas_Animate
#   define Canvas_Animate2                                 Retro_Canvas_DrawAnimation
#   define Canvas_DrawBox                                  Retro_Canvas_DrawBox
#   define Canvas_DrawRectangle                            Retro_Canvas_DrawRectangle
#   define Canvas_DrawPalette                              Retro_Canvas_DrawPalette
#   define Canvas_DrawPalette2                             Retro_Canvas_DrawPalette2
#   define Canvas_Print                                    Retro_Canvas_Print
#   define Canvas_Printf                                   Retro_Canvas_Printf
#   define Canvas_MonoPrint                                Retro_Canvas_MonoPrint
#endif










RETRO_API void  Retro_Audio_PlaySound(SoundHandle sound, U8 volume);

RETRO_API void  Retro_Audio_ClearSounds();

RETRO_API void  Retro_Audio_PlayMusic(const char* name);

RETRO_API void  Retro_Audio_StopMusic();

#if RETRO_NAMESPACES == 1
  const struct RETRO_Audio
  {
    void (*playSound)(SoundHandle sound, U8 volume);
    void (*clearSounds)();
    void (*playMusic)(const char* name);
    void (*stopMusic)();
  }
  RETRO_AUDIO_NAMESPACE_NAME
  = {
    Retro_Audio_PlaySound,
    Retro_Audio_ClearSounds,
    Retro_Audio_PlayMusic,
    Retro_Audio_StopMusic
  };
#endif

#if defined(RETRO_SHORTHAND)
#   define    Audio_PlaySound              Retro_Audio_PlaySound
#   define    Audio_ClearSounds            Retro_Audio_ClearSounds
#   define    Audio_PlayMusic              Retro_Audio_PlayMusic
#   define    Audio_StopMusic              Retro_Audio_StopMusic
#endif

RETRO_API void   Retro_Palette_Add(Colour colour);

RETRO_API void   Retro_Palette_AddRGB(U8 r, U8 g, U8 b);

RETRO_API U8     Retro_Palette_Index(Colour colour);

RETRO_API bool   Retro_Palette_Has(Colour colour);

RETRO_API Colour Retro_Palette_Get(U8 index);

RETRO_API void   Retro_Palette_Set(U8 index, Colour colour);

RETRO_API void   Retro_Palette_SetRGB(U8 index, U8 r, U8 g, U8 b);

#if RETRO_NAMESPACES == 1
  const struct RETRO_Palette {
    void   (*add)(Colour colour);
    void   (*addRGB)(U8 r, U8 g, U8 b);
    U8     (*index)(Colour colour);
    bool   (*has)(Colour colour);
    Colour (*get)(U8 index);
    void   (*set)(U8 index, Colour colour);
    void   (*setRGB)(U8 index, U8 r, U8 g, U8 b);
#if (RETRO_DEFAULT_PALETTE == 'DB16' || RETRO_DEFAULT_PALETTE == 'db16')
    U8 black, darkRed, darkBlue, darkGray, brown, darkGreen, red, lightGray, 
       lightBlue, orange, blueGray, lightGreen, peach, cyan, yellow, white,
       transparent;
#endif
  }
  RETRO_PALETTE_NAMESPACE_NAME
  = {
    Retro_Palette_Add,
    Retro_Palette_AddRGB,
    Retro_Palette_Index,
    Retro_Palette_Has,
    Retro_Palette_Get,
    Retro_Palette_Set,
    Retro_Palette_SetRGB
    #if (RETRO_DEFAULT_PALETTE == 'DB16' || RETRO_DEFAULT_PALETTE == 'db16')
    ,  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    #endif
  };
#endif

#if defined(RETRO_SHORTHAND)
#   define  Palette_Add            Retro_Palette_Add
#   define  Palette_Add2           Retro_Palette_Add2
#   define  Palette_AddRGB         Retro_Palette_AddARGB
#   define  Palette_Index          Retro_Palette_Index
#   define  Palette_Has            Retro_Palette_Has
#   define  Palette_Get            Retro_Palette_Get
#endif









RETRO_API int   Retro_Input_TextInput(char* str, U32 capacity);

RETRO_API void  Retro_Input_BindKey(int sdl_scancode, int action);

RETRO_API void  Retro_Input_BindAxis(int axis, int action);

RETRO_API bool  Retro_Input_Down(int action);

RETRO_API bool  Retro_Input_Released(int action);

RETRO_API bool  Retro_Input_Pressed(int action);

RETRO_API S16   Retro_Input_Axis(int action);

RETRO_API S16   Retro_Input_DeltaAxis(int action);

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
    Retro_Input_TextInput,
    Retro_Input_BindKey,
    Retro_Input_BindAxis,
    Retro_Input_Down,
    Retro_Input_Released,
    Retro_Input_Pressed,
    Retro_Input_Axis,
    Retro_Input_DeltaAxis
  };
#endif

#if defined(RETRO_SHORTHAND)
#   define Input_TextInput           Retro_Input_TextInput
#   define Input_BindKey             Retro_Input_BindKey
#   define Input_BindAxis            Retro_Input_BindAxis
#   define Input_Down                Retro_Input_Down
#   define Input_Released            Retro_Input_Released
#   define Input_Pressed             Retro_Input_Pressed
#   define Input_NowAxis             Retro_Input_NowAxis
#   define Input_DeltaAxis           Retro_Input_DeltaAxis

#if (RETRO_DEFAULT_PALETTE == 'DB16' || RETRO_DEFAULT_PALETTE == 'db16')
#   define Colour_black 0 
#   define Colour_darkRed 1 
#   define Colour_darkBlue 2 
#   define Colour_darkGray 3 
#   define Colour_brown 4 
#   define Colour_darkGreen 5 
#   define Colour_red 6 
#   define Colour_lightGray 7 
#   define Colour_lightBlue 8 
#   define Colour_orange 9 
#   define Colour_blueGray 10 
#   define Colour_lightGreen 11 
#   define Colour_peach 12 
#   define Colour_cyan 13 
#   define Colour_yellow 14 
#   define Colour_white 15 
#   define Colour_transparent 16 
#endif

#endif












RETRO_API void  Retro_Timer_Make(Timer* timer);

RETRO_API void  Retro_Timer_Start(Timer* timer);

RETRO_API void  Retro_Timer_Stop(Timer* timer);

RETRO_API void  Retro_Timer_Pause(Timer* timer);

RETRO_API void  Retro_Timer_Unpause(Timer* timer);

RETRO_API U32   Retro_Timer_Ticks(Timer* timer);

RETRO_API bool  Retro_Timer_Started(Timer* timer);

RETRO_API bool  Retro_Timer_Paused(Timer* timer);

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
RETRO_TIMER_NAMESPACE_NAME
  = {
    Retro_Timer_Make,
    Retro_Timer_Start,
    Retro_Timer_Stop,
    Retro_Timer_Pause,
    Retro_Timer_Unpause,
    Retro_Timer_Ticks,
    Retro_Timer_Started,
    Retro_Timer_Paused,
  };
#endif

#if defined(RETRO_SHORTHAND)
#   define Timer_Make(TIMER)      Retro_TimerMake(TIMER)
#   define Timer_Start(TIMER)     Retro_Timer_Start(TIMER)
#   define Timer_Stop(TIMER)      Retro_Timer_Stop(TIMER)
#   define Timer_Pause(TIMER)     Retro_Timer_Pause(TIMER)
#   define Timer_Unpause(TIMER)   Retro_Timer_Unpause(TIMER)
#   define Timer_Ticks(TIMER)     Retro_Timer_Ticks(TIMER)
#   define Timer_Started(TIMER)   Retro_Timer_Started(TIMER)
#   define Timer_Paused(TIMER)    Retro_Timer_Paused(TIMER)
#endif

#endif
