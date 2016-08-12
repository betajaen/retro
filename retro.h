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
  S32 left, top, right, bottom;
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
  SDL_Rect        frames[RETRO_MAX_ANIMATED_SPRITE_FRAMES];
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
} StaticSpriteObject;

typedef struct
{
  S32        x, y;
  U8         w, h;
  U8         frameNumber, flags;
  U8         animationHandle;
  U16        frameTime;
} AnimatedSpriteObject;

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
  U8      canvasWidth;
  U8      canvasHeight;
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

Size Size_Make(U32 w, U32 h);

void Arena_Save(const char* filename);

void Arena_Load(const char* filename, bool loadMusic);

U8* Arena_SaveToMem(U32* size);

void Arena_LoadFromMem(U8* mem, bool loadMusic);

void Scope_Push(int name);

int  Scope_GetName();

U8* Scope_Obtain(U32 size);

#define Scope_New(T) ((T*) Scope_Obtain(sizeof(T)))

void Scope_Rewind();

void Scope_Pop();

Colour Colour_Make(U8 r, U8 g, U8 b);

void* Resource_Load(const char* name, U32* outSize);

// Loads a bitmap and matches the palette to the canvas palette best it can.
void  Bitmap_Load(const char* name, Bitmap* outBitmap, U8 transparentIndex);

// Loads a bitmap with the palette order matching exactly the canvas palette.
// Offset is given to offset the loaded order by N colours
void  Bitmap_LoadPaletted(const char* name, Bitmap* outBitmap, U8 colourOffset);

void  Sprite_Make(Sprite* inSprite, Bitmap* bitmap, U32 x, U32 y, U32 w, U32 h);

Sprite* SpriteHandle_Get(SpriteHandle id);

void  Animation_LoadHorizontal(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

void  Animation_LoadVertical(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

Animation* AnimationHandle_Get(AnimationHandle id);

Size  Screen_GetSize();

S32   Canvas_GetWidth();

S32   Canvas_GetHeight();

void  Canvas_Set(U8 id);

typedef enum
{
  // Clear each frame
  CNF_Clear      = 1,
  // Apply blending to the canvas underneath
  CNF_Blend      = 2,
  // Render this canvas
  CNF_Render     = 4,
} CanvasFlags;

void  Canvas_SetFlags(U8 id, U8 flags, U8 clearColour);

void  Canvas_Splat(Bitmap* bitmap, S32 x, S32 y, Rect* srcRectangle);

void  Canvas_Splat2(Bitmap* bitmap, S32 x, S32 y, SDL_Rect* srcRectangle);

void  Canvas_Splat3(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle);

typedef enum
{
  FF_None     = SDL_FLIP_NONE,
  FF_FlipHorz = SDL_FLIP_HORIZONTAL,
  FF_FlipVert = SDL_FLIP_VERTICAL,
  FF_FlipDiag = FF_FlipHorz | FF_FlipVert,
  FF_Mask     = FF_FlipHorz | FF_FlipVert
} FlipFlags;

void  Canvas_SplatFlip(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle, U8 flipFlags);

void  Canvas_Place(StaticSpriteObject* spriteObject);

void  Canvas_Place2(Sprite* sprite, S32 x, S32 y);

void  Canvas_PlaceAnimated(AnimatedSpriteObject* spriteObject, bool updateTiming);

void  Canvas_PlaceAnimated2(Animation* animatedSprite, S32 x, S32 y, U8 frame, U8 flipFlags);

void  Canvas_PlaceScaled(Sprite* sprite, U32 x, U32 y, U32 scale);

void  Canvas_PlaceScaledF(Sprite* sprite, U32 x, U32 y, float scale);

void  Canvas_Flip();

void  Canvas_Clear();

void  Canvas_DrawPalette(Palette* palette, U32 Y);

void  Canvas_DrawRectangle(U8 Colour, Rect rect);

void  Canvas_DrawFilledRectangle(U8 Colour, Rect rect);

void  Canvas_PrintF(U32 x, U32 y, Font* font, U8 colour, const char* fmt, ...);

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
} FramePresentation;

void  Canvas_SetPresentation(FramePresentation presentation, float alpha, float beta);

void  AnimatedSpriteObject_Make(AnimatedSpriteObject* inAnimatedSpriteObject, Animation* animation, S32 x, S32 y);

void  AnimatedSpriteObject_PlayAnimation(AnimatedSpriteObject* animatedSpriteObject, bool playing, bool loop);

void  AnimatedSpriteObject_SwitchAnimation(AnimatedSpriteObject* animatedSpriteObject, Animation* newAnimation, bool animate);

void  Sound_Load(Sound* sound, const char* name);

void  Sound_Play(Sound* sound, U8 volume);

void  Sound_Clear();

void  Music_Play(const char* name);

void  Music_Stop();

void  Palette_Make(Palette* palette);

void  Palette_SetDefault(const Palette* palette);

void  Palette_LoadFromBitmap(const char* name, Palette* palette);

void  Palette_Add(Palette* palette, Colour colour);

void  Palette_AddARGBInt(Palette* palette, U32 argb);

U8    Palette_FindColour(Palette* palette, Colour colour);

bool  Palette_HasColour(Palette* palette, Colour colour);

void  Palette_CopyTo(const Palette* src, Palette* dst);

#define Palette_GetColour(PALETTE, INDEX) \
  ((PALETTE)->colours[INDEX >= (PALETTE)->count ? (PALETTE)->fallback : INDEX])

void  Font_Make(Font* font);

void  Font_Load(const char* name, Font* font, Colour markerColour, Colour transparentColour);

int   Input_TextInput(char* str, U32 capacity);

void  Input_BindKey(int key, int action);

void  Input_BindAxis(int axis, int action);

bool  Input_GetActionDown(int action);

bool  Input_GetActionReleased(int action);

bool  Input_GetActionPressed(int action);

S16   Input_GetActionNowAxis(int action);

S16   Input_GetActionDeltaAxis(int action);

void  Timer_Make(Timer* timer);

void  Timer_Start(Timer* timer);

void  Timer_Stop(Timer* timer);

void  Timer_Pause(Timer* timer);

void  Timer_Unpause(Timer* timer);

U32   Timer_GetTicks(Timer* timer);

bool  Timer_IsStarted(Timer* timer);

bool  Timer_IsPaused(Timer* timer);

void  Init(Settings* s);

void  Start();

void  Step();

#endif
