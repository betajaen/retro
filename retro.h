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

#ifndef RETRO_DEFAULT_CANVAS_SCALE
#define RETRO_DEFAULT_CANVAS_SCALE 2
#endif

#ifndef RETRO_DEFAULT_CANVAS_X
#define RETRO_DEFAULT_CANVAS_X 0xFFFFffff
#endif

#ifndef RETRO_DEFAULT_CANVAS_Y
#define RETRO_DEFAULT_CANVAS_Y 0xFFFFffff
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

#ifndef RETRO_DEFAULT_PALETTE
#define RETRO_DEFAULT_PALETTE 'DB16'
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
#         define RETRO_USER_SETTINGS_API  __declspec(dllexport)
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

typedef U16 Retro_SpriteHandle;
typedef U16 Retro_AnimationHandle;
typedef U16 Retro_BitmapHandle;
typedef U16 Retro_SoundHandle;

typedef struct
{
  S32 x, y;
} Retro_Point;

typedef struct
{
  S32 w, h;
} Retro_Size;

typedef struct
{
  S32 x, y, w, h;
} Retro_Rect;

typedef struct
{
  U8 r, g, b, a;
} Retro_Colour;

typedef struct
{
  Retro_Colour colours[256];
  U16    count;
  U8     fallback, transparent;
} Retro_Palette;

typedef struct
{
  Retro_BitmapHandle bitmap;
  U8           height;
  U16          x[256];
  U8           widths[256];
} Retro_Font;

typedef struct
{
  U32    start, paused;
  U8     flags;
} Retro_Timer;

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
} Retro_SpriteObjectFlags;

typedef struct
{
  S32  x, y;
  S8   flags;
  U8   spriteHandle;
} Retro_SpriteObject;

typedef struct
{
  S32        x, y;
  U8         w, h;
  U8         frameNumber, flags;
  U8         animationHandle;
  U16        frameTime;
} Retro_AnimationObject;

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

RETRO_API U8           Retro_Hex_Char(char str);
RETRO_API U8           Retro_Hex_U8(const char* str);
RETRO_API S8           Retro_Hex_S8(const char* str);
RETRO_API U16          Retro_Hex_U16(const char* str);
RETRO_API S16          Retro_Hex_S16(const char* str);
RETRO_API U32          Retro_Hex_U32(const char* str);
RETRO_API S32          Retro_Hex_S32(const char* str);
RETRO_API Retro_Colour Retro_Hex_Colour(const char* str);
RETRO_API Retro_Colour Retro_Colour_Make(U8 r, U8 g, U8 b);
RETRO_API Retro_Point  Retro_Point_Make(S32 x, S32 y);
RETRO_API Retro_Size   Retro_Size_Make(U32 w, U32 h);
RETRO_API Retro_Rect   Retro_Rect_Make(S32 x, S32 y, S32 w, S32 h);





typedef struct
{
  const char* caption;
  U32         windowWidth, windowHeight;
  S32         canvasX, canvasY;
  U32         canvasWidth, canvasHeight;
  float       canvasScaleX, canvasScaleY;
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
  RETRO_DEFAULT_CANVAS_X,
  RETRO_DEFAULT_CANVAS_Y,
  RETRO_DEFAULT_CANVAS_WIDTH,
  RETRO_DEFAULT_CANVAS_HEIGHT,
  RETRO_DEFAULT_CANVAS_SCALE,
  RETRO_DEFAULT_CANVAS_SCALE,
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




RETRO_API void Retro_Shutdown();




RETRO_API void Retro_Arena_LoadFromMemory(U8* mem, bool loadMusic);

RETRO_API void Retro_Arena_Load(const char* filename, bool loadMusic);

RETRO_API U8* Retro_Arena_SaveToMemory(U32* outSize);

RETRO_API void Retro_Arena_Save(const char* filename);







RETRO_API void Retro_Debug(Retro_Font* font);

RETRO_API void Retro_Scope_Push(int name);

RETRO_API int  Retro_Scope_Name();

RETRO_API U8* Retro_Scope_Obtain(U32 size);

RETRO_API void Retro_Scope_Rewind();

RETRO_API void Retro_Scope_Pop();

#define Retro_Scope_New(T) ((T*) Retro_Scope_Obtain(sizeof(T)))









// Loads a palette from the colours given in a image file.
RETRO_API void  Retro_Resources_LoadPalette(const char* name);

// Loads a bitmap and matches the palette to the canvas palette best it can.
RETRO_API Retro_BitmapHandle  Retro_Resources_LoadBitmap(const char* name, U8 transparentIndex);

// Load a sound into Sound
RETRO_API Retro_SoundHandle Retro_Resources_LoadSound(const char* name);

// Load a font using the Grafx2 format.
RETRO_API void  Retro_Resources_LoadFont(const char* name, Retro_Font* inFont, Retro_Colour markerColour, Retro_Colour transparentColour);

// Load a font that has a fixed width and arranged horizontally - like GrafX2 (but without the marker line)
RETRO_API void  Retro_Resources_LoadFontFixed(const char* name, Retro_Font* inFont, U8 w, Retro_Colour transparentColour);








RETRO_API Retro_SpriteHandle Retro_Sprites_LoadSprite(Retro_BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h);

RETRO_API Retro_AnimationHandle Retro_Sprites_LoadAnimationH(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

RETRO_API Retro_AnimationHandle Retro_Sprites_LoadAnimationV(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);

RETRO_API void  Retro_Sprites_NewAnimationObject(Retro_AnimationObject* inAnimatedSpriteObject, Retro_AnimationHandle animation, S32 x, S32 y);

RETRO_API void  Retro_Sprites_PlayAnimationObject(Retro_AnimationObject* animatedSpriteObject, bool playing, bool loop);

RETRO_API void  Retro_Sprites_SetAnimationObject(Retro_AnimationObject* animatedSpriteObject, Retro_AnimationHandle newAnimation, bool animate);







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

RETRO_API void  Retro_Canvas_Copy(Retro_BitmapHandle bitmap, Retro_Rect* dstRectangle, Retro_Rect* srcRectangle, U8 copyFlags);

RETRO_API void  Retro_Canvas_Copy2(Retro_BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);

RETRO_API void  Retro_Canvas_Sprite(Retro_SpriteObject* spriteObject);

RETRO_API void  Retro_Canvas_Animate(Retro_AnimationObject* animationObject, bool updateTiming);

RETRO_API void  Retro_Canvas_DrawPalette(S32 y);

RETRO_API void  Retro_Canvas_DrawPalette2(S32 y, U8 from, U8 to);

RETRO_API void  Retro_Canvas_DrawBox(U8 colour, Retro_Rect rect);

RETRO_API void  Retro_Canvas_DrawRectangle(U8 colour, Retro_Rect rect);

RETRO_API void  Retro_Canvas_DrawSprite(Retro_SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);

RETRO_API void  Retro_Canvas_DrawAnimation(Retro_AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);

RETRO_API void  Retro_Canvas_Print(S32 x, S32 y, Retro_Font* font, U8 colour, const char* str);

RETRO_API void  Retro_Canvas_Printf(S32 x, S32 y, Retro_Font* font, U8 colour, const char* fmt, ...);

RETRO_API void  Retro_Canvas_MonoPrint(S32 x, S32 y, S32 w, Retro_Font* font, U8 colour, const char* str);








RETRO_API void  Retro_Audio_PlaySound(Retro_SoundHandle sound, U8 volume);

RETRO_API void  Retro_Audio_ClearSounds();

RETRO_API void  Retro_Audio_PlayMusic(const char* name);

RETRO_API void  Retro_Audio_StopMusic();









RETRO_API void   Retro_Palette_Add(Retro_Colour colour);

RETRO_API void   Retro_Palette_AddRGB(U8 r, U8 g, U8 b);

RETRO_API U8     Retro_Palette_Index(Retro_Colour colour);

RETRO_API bool   Retro_Palette_Has(Retro_Colour colour);

RETRO_API Retro_Colour Retro_Palette_Get(U8 index);

RETRO_API void   Retro_Palette_Set(U8 index, Retro_Colour colour);

RETRO_API void   Retro_Palette_SetRGB(U8 index, U8 r, U8 g, U8 b);







  // The values are taken from the SDL_scancode.h file
  typedef enum
  {
    RETRO_KEY_UNKNOWN = 0,

    /**
    *  \name Usage page 0x07
    *
    *  These values are from usage page 0x07 (USB keyboard page).
    */
    /* @{ */

    RETRO_KEY_A = 4,
    RETRO_KEY_B = 5,
    RETRO_KEY_C = 6,
    RETRO_KEY_D = 7,
    RETRO_KEY_E = 8,
    RETRO_KEY_F = 9,
    RETRO_KEY_G = 10,
    RETRO_KEY_H = 11,
    RETRO_KEY_I = 12,
    RETRO_KEY_J = 13,
    RETRO_KEY_K = 14,
    RETRO_KEY_L = 15,
    RETRO_KEY_M = 16,
    RETRO_KEY_N = 17,
    RETRO_KEY_O = 18,
    RETRO_KEY_P = 19,
    RETRO_KEY_Q = 20,
    RETRO_KEY_R = 21,
    RETRO_KEY_S = 22,
    RETRO_KEY_T = 23,
    RETRO_KEY_U = 24,
    RETRO_KEY_V = 25,
    RETRO_KEY_W = 26,
    RETRO_KEY_X = 27,
    RETRO_KEY_Y = 28,
    RETRO_KEY_Z = 29,

    RETRO_KEY_1 = 30,
    RETRO_KEY_2 = 31,
    RETRO_KEY_3 = 32,
    RETRO_KEY_4 = 33,
    RETRO_KEY_5 = 34,
    RETRO_KEY_6 = 35,
    RETRO_KEY_7 = 36,
    RETRO_KEY_8 = 37,
    RETRO_KEY_9 = 38,
    RETRO_KEY_0 = 39,

    RETRO_KEY_RETURN = 40,
    RETRO_KEY_ESCAPE = 41,
    RETRO_KEY_BACKSPACE = 42,
    RETRO_KEY_TAB = 43,
    RETRO_KEY_SPACE = 44,

    RETRO_KEY_MINUS = 45,
    RETRO_KEY_EQUALS = 46,
    RETRO_KEY_LEFTBRACKET = 47,
    RETRO_KEY_RIGHTBRACKET = 48,
    RETRO_KEY_BACKSLASH = 49, /**< Located at the lower left of the return
                              *   key on ISO keyboards and at the right end
                              *   of the QWERTY row on ANSI keyboards.
                              *   Produces REVERSE SOLIDUS (backslash) and
                              *   VERTICAL LINE in a US layout, REVERSE
                              *   SOLIDUS and VERTICAL LINE in a UK Mac
                              *   layout, NUMBER SIGN and TILDE in a UK
                              *   Windows layout, DOLLAR SIGN and POUND SIGN
                              *   in a Swiss German layout, NUMBER SIGN and
                              *   APOSTROPHE in a German layout, GRAVE
                              *   ACCENT and POUND SIGN in a French Mac
                              *   layout, and ASTERISK and MICRO SIGN in a
                              *   French Windows layout.
                              */
    RETRO_KEY_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                              *   instead of 49 for the same key, but all
                              *   OSes I've seen treat the two codes
                              *   identically. So, as an implementor, unless
                              *   your keyboard generates both of those
                              *   codes and your OS treats them differently,
                              *   you should generate RETRO_KEY_BACKSLASH
                              *   instead of this code. As a user, you
                              *   should not rely on this code because SDL
                              *   will never generate it with most (all?)
                              *   keyboards.
                              */
    RETRO_KEY_SEMICOLON = 51,
    RETRO_KEY_APOSTROPHE = 52,
    RETRO_KEY_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                          *   and ISO keyboards). Produces GRAVE ACCENT and
                          *   TILDE in a US Windows layout and in US and UK
                          *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                          *   and NOT SIGN in a UK Windows layout, SECTION
                          *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                          *   layouts on ISO keyboards, SECTION SIGN and
                          *   DEGREE SIGN in a Swiss German layout (Mac:
                          *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                          *   DEGREE SIGN in a German layout (Mac: only on
                          *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                          *   French Windows layout, COMMERCIAL AT and
                          *   NUMBER SIGN in a French Mac layout on ISO
                          *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                          *   SIGN in a Swiss German, German, or French Mac
                          *   layout on ANSI keyboards.
                          */
    RETRO_KEY_COMMA = 54,
    RETRO_KEY_PERIOD = 55,
    RETRO_KEY_SLASH = 56,

    RETRO_KEY_CAPSLOCK = 57,

    RETRO_KEY_F1 = 58,
    RETRO_KEY_F2 = 59,
    RETRO_KEY_F3 = 60,
    RETRO_KEY_F4 = 61,
    RETRO_KEY_F5 = 62,
    RETRO_KEY_F6 = 63,
    RETRO_KEY_F7 = 64,
    RETRO_KEY_F8 = 65,
    RETRO_KEY_F9 = 66,
    RETRO_KEY_F10 = 67,
    RETRO_KEY_F11 = 68,
    RETRO_KEY_F12 = 69,

    RETRO_KEY_PRINTSCREEN = 70,
    RETRO_KEY_SCROLLLOCK = 71,
    RETRO_KEY_PAUSE = 72,
    RETRO_KEY_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                           does send code 73, not 117) */
    RETRO_KEY_HOME = 74,
    RETRO_KEY_PAGEUP = 75,
    RETRO_KEY_DELETE = 76,
    RETRO_KEY_END = 77,
    RETRO_KEY_PAGEDOWN = 78,
    RETRO_KEY_RIGHT = 79,
    RETRO_KEY_LEFT = 80,
    RETRO_KEY_DOWN = 81,
    RETRO_KEY_UP = 82,

    RETRO_KEY_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                 */
    RETRO_KEY_KP_DIVIDE = 84,
    RETRO_KEY_KP_MULTIPLY = 85,
    RETRO_KEY_KP_MINUS = 86,
    RETRO_KEY_KP_PLUS = 87,
    RETRO_KEY_KP_ENTER = 88,
    RETRO_KEY_KP_1 = 89,
    RETRO_KEY_KP_2 = 90,
    RETRO_KEY_KP_3 = 91,
    RETRO_KEY_KP_4 = 92,
    RETRO_KEY_KP_5 = 93,
    RETRO_KEY_KP_6 = 94,
    RETRO_KEY_KP_7 = 95,
    RETRO_KEY_KP_8 = 96,
    RETRO_KEY_KP_9 = 97,
    RETRO_KEY_KP_0 = 98,
    RETRO_KEY_KP_PERIOD = 99,

    RETRO_KEY_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                    *   keyboards have over ANSI ones,
                                    *   located between left shift and Y.
                                    *   Produces GRAVE ACCENT and TILDE in a
                                    *   US or UK Mac layout, REVERSE SOLIDUS
                                    *   (backslash) and VERTICAL LINE in a
                                    *   US or UK Windows layout, and
                                    *   LESS-THAN SIGN and GREATER-THAN SIGN
                                    *   in a Swiss German, German, or French
                                    *   layout. */
    RETRO_KEY_APPLICATION = 101, /**< windows contextual menu, compose */
    RETRO_KEY_POWER = 102, /**< The USB document says this is a status flag,
                           *   not a physical key - but some Mac keyboards
                           *   do have a power key. */
    RETRO_KEY_KP_EQUALS = 103,
    RETRO_KEY_F13 = 104,
    RETRO_KEY_F14 = 105,
    RETRO_KEY_F15 = 106,
    RETRO_KEY_F16 = 107,
    RETRO_KEY_F17 = 108,
    RETRO_KEY_F18 = 109,
    RETRO_KEY_F19 = 110,
    RETRO_KEY_F20 = 111,
    RETRO_KEY_F21 = 112,
    RETRO_KEY_F22 = 113,
    RETRO_KEY_F23 = 114,
    RETRO_KEY_F24 = 115,
    RETRO_KEY_EXECUTE = 116,
    RETRO_KEY_HELP = 117,
    RETRO_KEY_MENU = 118,
    RETRO_KEY_SELECT = 119,
    RETRO_KEY_STOP = 120,
    RETRO_KEY_AGAIN = 121,   /**< redo */
    RETRO_KEY_UNDO = 122,
    RETRO_KEY_CUT = 123,
    RETRO_KEY_COPY = 124,
    RETRO_KEY_PASTE = 125,
    RETRO_KEY_FIND = 126,
    RETRO_KEY_MUTE = 127,
    RETRO_KEY_VOLUMEUP = 128,
    RETRO_KEY_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     RETRO_KEY_LOCKINGCAPSLOCK = 130,  */
    /*     RETRO_KEY_LOCKINGNUMLOCK = 131, */
    /*     RETRO_KEY_LOCKINGSCROLLLOCK = 132, */
    RETRO_KEY_KP_COMMA = 133,
    RETRO_KEY_KP_EQUALSAS400 = 134,

    RETRO_KEY_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                    footnotes in USB doc */
    RETRO_KEY_INTERNATIONAL2 = 136,
    RETRO_KEY_INTERNATIONAL3 = 137, /**< Yen */
    RETRO_KEY_INTERNATIONAL4 = 138,
    RETRO_KEY_INTERNATIONAL5 = 139,
    RETRO_KEY_INTERNATIONAL6 = 140,
    RETRO_KEY_INTERNATIONAL7 = 141,
    RETRO_KEY_INTERNATIONAL8 = 142,
    RETRO_KEY_INTERNATIONAL9 = 143,
    RETRO_KEY_LANG1 = 144, /**< Hangul/English toggle */
    RETRO_KEY_LANG2 = 145, /**< Hanja conversion */
    RETRO_KEY_LANG3 = 146, /**< Katakana */
    RETRO_KEY_LANG4 = 147, /**< Hiragana */
    RETRO_KEY_LANG5 = 148, /**< Zenkaku/Hankaku */
    RETRO_KEY_LANG6 = 149, /**< reserved */
    RETRO_KEY_LANG7 = 150, /**< reserved */
    RETRO_KEY_LANG8 = 151, /**< reserved */
    RETRO_KEY_LANG9 = 152, /**< reserved */

    RETRO_KEY_ALTERASE = 153, /**< Erase-Eaze */
    RETRO_KEY_SYSREQ = 154,
    RETRO_KEY_CANCEL = 155,
    RETRO_KEY_CLEAR = 156,
    RETRO_KEY_PRIOR = 157,
    RETRO_KEY_RETURN2 = 158,
    RETRO_KEY_SEPARATOR = 159,
    RETRO_KEY_OUT = 160,
    RETRO_KEY_OPER = 161,
    RETRO_KEY_CLEARAGAIN = 162,
    RETRO_KEY_CRSEL = 163,
    RETRO_KEY_EXSEL = 164,

    RETRO_KEY_KP_00 = 176,
    RETRO_KEY_KP_000 = 177,
    RETRO_KEY_THOUSANDSSEPARATOR = 178,
    RETRO_KEY_DECIMALSEPARATOR = 179,
    RETRO_KEY_CURRENCYUNIT = 180,
    RETRO_KEY_CURRENCYSUBUNIT = 181,
    RETRO_KEY_KP_LEFTPAREN = 182,
    RETRO_KEY_KP_RIGHTPAREN = 183,
    RETRO_KEY_KP_LEFTBRACE = 184,
    RETRO_KEY_KP_RIGHTBRACE = 185,
    RETRO_KEY_KP_TAB = 186,
    RETRO_KEY_KP_BACKSPACE = 187,
    RETRO_KEY_KP_A = 188,
    RETRO_KEY_KP_B = 189,
    RETRO_KEY_KP_C = 190,
    RETRO_KEY_KP_D = 191,
    RETRO_KEY_KP_E = 192,
    RETRO_KEY_KP_F = 193,
    RETRO_KEY_KP_XOR = 194,
    RETRO_KEY_KP_POWER = 195,
    RETRO_KEY_KP_PERCENT = 196,
    RETRO_KEY_KP_LESS = 197,
    RETRO_KEY_KP_GREATER = 198,
    RETRO_KEY_KP_AMPERSAND = 199,
    RETRO_KEY_KP_DBLAMPERSAND = 200,
    RETRO_KEY_KP_VERTICALBAR = 201,
    RETRO_KEY_KP_DBLVERTICALBAR = 202,
    RETRO_KEY_KP_COLON = 203,
    RETRO_KEY_KP_HASH = 204,
    RETRO_KEY_KP_SPACE = 205,
    RETRO_KEY_KP_AT = 206,
    RETRO_KEY_KP_EXCLAM = 207,
    RETRO_KEY_KP_MEMSTORE = 208,
    RETRO_KEY_KP_MEMRECALL = 209,
    RETRO_KEY_KP_MEMCLEAR = 210,
    RETRO_KEY_KP_MEMADD = 211,
    RETRO_KEY_KP_MEMSUBTRACT = 212,
    RETRO_KEY_KP_MEMMULTIPLY = 213,
    RETRO_KEY_KP_MEMDIVIDE = 214,
    RETRO_KEY_KP_PLUSMINUS = 215,
    RETRO_KEY_KP_CLEAR = 216,
    RETRO_KEY_KP_CLEARENTRY = 217,
    RETRO_KEY_KP_BINARY = 218,
    RETRO_KEY_KP_OCTAL = 219,
    RETRO_KEY_KP_DECIMAL = 220,
    RETRO_KEY_KP_HEXADECIMAL = 221,

    RETRO_KEY_LCTRL = 224,
    RETRO_KEY_LSHIFT = 225,
    RETRO_KEY_LALT = 226, /**< alt, option */
    RETRO_KEY_LGUI = 227, /**< windows, command (apple), meta */
    RETRO_KEY_RCTRL = 228,
    RETRO_KEY_RSHIFT = 229,
    RETRO_KEY_RALT = 230, /**< alt gr, option */
    RETRO_KEY_RGUI = 231, /**< windows, command (apple), meta */

    RETRO_KEY_MODE = 257,    /**< I'm not sure if this is really not covered
                             *   by any of the above, but since there's a
                             *   special KMOD_MODE for it I'm adding it here
                             */

                             /* @} *//* Usage page 0x07 */

                                     /**
                                     *  \name Usage page 0x0C
                                     *
                                     *  These values are mapped from usage page 0x0C (USB consumer page).
                                     */
                                     /* @{ */

                                     RETRO_KEY_AUDIONEXT = 258,
                                     RETRO_KEY_AUDIOPREV = 259,
                                     RETRO_KEY_AUDIOSTOP = 260,
                                     RETRO_KEY_AUDIOPLAY = 261,
                                     RETRO_KEY_AUDIOMUTE = 262,
                                     RETRO_KEY_MEDIASELECT = 263,
                                     RETRO_KEY_WWW = 264,
                                     RETRO_KEY_MAIL = 265,
                                     RETRO_KEY_CALCULATOR = 266,
                                     RETRO_KEY_COMPUTER = 267,
                                     RETRO_KEY_AC_SEARCH = 268,
                                     RETRO_KEY_AC_HOME = 269,
                                     RETRO_KEY_AC_BACK = 270,
                                     RETRO_KEY_AC_FORWARD = 271,
                                     RETRO_KEY_AC_STOP = 272,
                                     RETRO_KEY_AC_REFRESH = 273,
                                     RETRO_KEY_AC_BOOKMARKS = 274,

                                     /* @} *//* Usage page 0x0C */

                                             /**
                                             *  \name Walther keys
                                             *
                                             *  These are values that Christian Walther added (for mac keyboard?).
                                             */
                                             /* @{ */

                                             RETRO_KEY_BRIGHTNESSDOWN = 275,
                                             RETRO_KEY_BRIGHTNESSUP = 276,
                                             RETRO_KEY_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                                                            switch, video mode switch */
    RETRO_KEY_KBDILLUMTOGGLE = 278,
    RETRO_KEY_KBDILLUMDOWN = 279,
    RETRO_KEY_KBDILLUMUP = 280,
    RETRO_KEY_EJECT = 281,
    RETRO_KEY_SLEEP = 282,

    RETRO_KEY_APP1 = 283,
    RETRO_KEY_APP2 = 284,

    /* @} *//* Walther keys */

            /* Add any other keys here. */

            RETRO_KEY_NUM_KEYS = 512 /**< not a key, just marks the number of scancodes
                                     for array bounds */
  } Retro_Key;

RETRO_API int   Retro_Input_TextInput(char* str, U32 capacity);

RETRO_API void  Retro_Input_BindKey(int retroKey, int action);

RETRO_API void  Retro_Input_BindAxis(int axis, int action);

RETRO_API bool  Retro_Input_Down(int action);

RETRO_API bool  Retro_Input_Released(int action);

RETRO_API bool  Retro_Input_Pressed(int action);

RETRO_API S16   Retro_Input_Axis(int action);

RETRO_API S16   Retro_Input_DeltaAxis(int action);








RETRO_API void  Retro_Timer_Make(Retro_Timer* timer);

RETRO_API void  Retro_Timer_Start(Retro_Timer* timer);

RETRO_API void  Retro_Timer_Stop(Retro_Timer* timer);

RETRO_API void  Retro_Timer_Pause(Retro_Timer* timer);

RETRO_API void  Retro_Timer_Unpause(Retro_Timer* timer);

RETRO_API U32   Retro_Timer_Ticks(Retro_Timer* timer);

RETRO_API bool  Retro_Timer_Started(Retro_Timer* timer);

RETRO_API bool  Retro_Timer_Paused(Retro_Timer* timer);









typedef enum
{
  RCF_None              = 0,
  RCF_LoadLibraryAsCopy = (1 << 0),
} RetroContextFlags;


// Current context name (i.e. Context in step function), or -1 for none.
RETRO_API S32   Retro_Context_Id();

// Get context count. Most common is 1, unless in the editor then probably 2.
RETRO_API U32   Retro_Context_Count();

// All context ids. outNames must be an array of at least of 'Retro_Context_Count()'
RETRO_API void  Retro_Context_Ids(int* outNames);

// Enable a context so it can 'Step'
RETRO_API void  Retro_Context_Enable(int name);

// Disable a context from 'Step'
RETRO_API void  Retro_Context_Disable(int name);

// Unload a context from memory (if a library also unload that library)
RETRO_API void  Retro_Context_Unload(int name);

// Load a context internally from the current runtime. Most common method
RETRO_API int   Retro_Context_LoadFromInternal(Retro_Settings* settings, void(*initFunction)(), void(*startFunction)(), void(*stepFunction)(), U8 contextFlags);

// Load a context externally i.e. from a library.  Least common method - Used with the editor, for edit-and-play
RETRO_API int   Retro_Context_LoadFromLibrary(const char* libraryPath, U8 contextFlags);






#if !(defined(RETRO_COMPILING_AS_LIBRARY) || defined(RETRO_USING_RETRO_AS_LIBRARY) )

  RETRO_USER_INIT_API  void Init();
  RETRO_USER_START_API void Start();
  RETRO_USER_STEP_API  void Step();

  #ifndef RETRO_NO_MAIN
  #define RETRO_NO_MAIN

    int main(int argc, char *argv[])
    {
      Retro_Settings settings = Retro_Default_Settings;
      Retro_Context_LoadFromInternal(&settings, Init, Start, Step, 0);
      Retro_Shutdown();
      return 0;
    }

  #endif

  #endif

#endif  // RETRO_H


