#ifndef RETRO_C
#define RETRO_C

#include "retro.h"

#ifndef RETRO_MAX_CONTEXT
#define RETRO_MAX_CONTEXT 4
#endif

#ifndef RETRO_DEFAULT_MAX_INPUT_BINDINGS
#define RETRO_DEFAULT_MAX_INPUT_BINDINGS 4
#endif

#ifndef RETRO_DEFAULT_MAX_ANIMATED_SPRITE_FRAMES
#define RETRO_DEFAULT_MAX_ANIMATED_SPRITE_FRAMES 8
#endif

#ifdef RETRO_WINDOWS
#   include "windows.h"
#endif

#define LODEPNG_NO_COMPILE_ENCODER
#include "ref/lodepng.c"
#include "ref/micromod.c"
#include "ref/micromod_sdl.c"
#include "SDL_main.h"
#include "assert.h"

#ifdef RETRO_BROWSER
#include <emscripten.h>
#endif

#include "SDL.h"

typedef struct
{
  U8 *begin, *end, *current;
} RetroP_Arena;

typedef struct
{
  int                name;
  U32                p;
} RetroP_ScopeStack;

typedef struct
{
  U32 action;
  int keys[RETRO_DEFAULT_MAX_INPUT_BINDINGS];
  int axis[RETRO_DEFAULT_MAX_INPUT_BINDINGS];
  S16 state, lastState;
} RetroP_InputActionBinding;

typedef enum
{
  ICS_None,
  ICS_Character,
  ICS_Backspace,
  ICS_Enter
} RetroP_InputCharState;

typedef struct
{
  SDL_AudioSpec     specification;
} RetroP_SoundDevice;

typedef struct
{
  SDL_Texture*  texture;
  U8*           imageData;
  U16           w, h;
  Retro_BitmapHandle  bitmapHandle;
} RetroP_Bitmap;

typedef struct
{
  U32 length;
  U8* buffer;
  SDL_AudioSpec spec;
  Retro_SoundHandle soundHandle;
} RetroP_Sound;

typedef struct
{
  RetroP_Sound* sound;
  U32    p;
  U8     volume;
} RetroP_SoundObject;

typedef struct
{
  Retro_BitmapHandle  bitmap;
  SDL_Rect      rect;
  Retro_SpriteHandle  spriteHandle;
} RetroP_Sprite;

typedef struct
{
  Retro_BitmapHandle    bitmap;
  U8              frameCount;
  U8              w, h;
  U16             frameLength;
  Retro_AnimationHandle animationHandle;
  Retro_Rect            frames[RETRO_DEFAULT_MAX_ANIMATED_SPRITE_FRAMES];
} RetroP_Animation;

typedef union
{
  U32  q;
  U16  w[2];
  U8   b[4];
} RetroFourByteUnion;

#ifdef  RETRO_BROWSER

#define RETRO_BROWSER_PATH ((const char*) (RetroCtx->tempBrowserPath))

#define RETRO_MAKE_BROWSER_PATH(N)          \
  RetroCtx->tempBrowserPath[0] = 0;             \
  strcat(RetroCtx->tempBrowserPath, "assets/"); \
  strcat(RetroCtx->tempBrowserPath, name)

#endif

typedef struct
{
  SDL_Texture* texture;
  U8           flags;
  U8           backgroundColour;
} RetroP_Canvas;

typedef struct {
#if defined(RETRO_WINDOWS)
  HANDLE winHandle;
  U8 __padding[16 - sizeof(HANDLE)];
#else
  U8 __padding[16];
#endif
  void(*initFunction)();
  void(*startFunction)();
  void(*stepFunction)();
  void(*settingsFunction)(Retro_Settings*);
} RetroP_Library;

typedef struct
{
  // Input
  char                         inputChar;
  RetroP_InputCharState        inputCharState;
  const U8*                    keyState;
} RetroP_SharedContext;

typedef struct
{
  // Memory and User Memory
  RetroP_Arena                 mem;
  RetroP_Arena                 arena;

  // Unique Context Id.
  U32                          id;
  bool                         enabled;

  // Library instance, and user functions (init, start and update)
  RetroP_Library               library;

  // User Settings
  Retro_Settings               settings;

  // User Scope
  RetroP_ScopeStack            scopeStack[256];
  U32                          scopeStackIndex;

  // Loaded and Parsed Resources
  RetroP_Bitmap*               bitmaps;
  RetroP_Sprite*               sprites;
  RetroP_Animation*            animations;
  RetroP_Sound*                sounds;

  // Canvas and Rendering
  Retro_Palette                      palette;

  SDL_Texture*                 mainTexture;
  SDL_Texture*                 canvasTexture;
  RetroP_Canvas*               canvas;
  Retro_CanvasPresentation     framePresentation;
  float                        frameAlpha, frameBeta;

  // Input
  RetroP_InputActionBinding*   inputActions;
  
  // Events and Timing
  bool                         quit;
  Retro_Timer                        fpsTimer,  deltaTimer;
  U32                          frameCount;
  U32                          deltaTime;
  U32                          waitTime;
  float                        fps;

  // Audio
  RetroP_SoundObject*          soundObject;

  // Platform specific
#ifdef RETRO_BROWSER
  char                         tempBrowserPath[256];
  U8*                          musicFileData;
#endif
} RetroP_Context;

RetroP_Context          *RetroCtx = NULL, 
                        *RetroLastCtx = NULL,
                        *RetroAudioCtx = NULL;
RetroP_SharedContext     RetroSharedCtx;
RetroP_Context*          RetroContexts[RETRO_MAX_CONTEXT] = { NULL };
U32                      RetroContextCount;

U32                      RetroPMinWaitTime;
Retro_Timer              RetroPCapTimer;

SDL_Window*              RetroPWindow;
SDL_Renderer*            RetroPRenderer;
RetroP_SoundDevice       RetroPAudioDevice;
micromod_sdl_context*    RetroPMusicContext;

U16                      RetroPWindowWidth;
U16                      RetroPWindowHeight;


#define RETRO_SDL_DRAW_PUSH_RGB(T, RGB) \
  SDL_Color T; U8 TAlpha;\
  SDL_GetRenderDrawColor(RetroPRenderer, &T.r, &T.g, &T.b, &TAlpha); \
  SDL_SetRenderDrawColor(RetroPRenderer, RGB.r, RGB.g, RGB.b, 0xFF);

#define RETRO_SDL_DRAW_POP_RGB(T) \
  SDL_SetRenderDrawColor(RetroPRenderer, T.r, T.g, T.b, 0xFF);

#define RETRO_SDL_TEXTURE_PUSH_RGB(T, TEXTURE, RGB) \
  SDL_Color T; \
  SDL_GetTextureColorMod(TEXTURE, &T.r, &T.g, &T.b); \
  SDL_SetTextureColorMod(TEXTURE, RGB.r, RGB.g, RGB.b);

#define RETRO_SDL_TEXTURE_POP_RGB(T, TEXTURE) \
  SDL_SetTextureColorMod(TEXTURE, T.r, T.g, T.b);

#define RETRO_SDL_TO_RECT(SRC, DST) \
  DST.x = SRC.x;\
  DST.y = SRC.y;\
  DST.w = SRC.w;\
  DST.h = SRC.h;

void* Retro_Resource_Load(const char* name, U32* outSize)
{
#ifdef RETRO_WINDOWS
  assert(outSize);

  HRSRC handle = FindResource(RetroCtx->library.winHandle, name, "RESOURCE");
  assert(handle);

  HGLOBAL data = LoadResource(RetroCtx->library.winHandle, handle);
  assert(data);

  void* ptr = LockResource(data);
  assert(ptr);

  DWORD dataSize = SizeofResource(RetroCtx->library.winHandle, handle);
  assert(dataSize);

  (*outSize) = dataSize;

  return ptr;
#else
  RETRO_UNUSED(name);
  RETRO_UNUSED(outSize);
  return NULL;
#endif
}


void  Retro_Resources_LoadPalette(const char* name)
{
  U32 width, height;

  U8* imageData = NULL;

  char n[256];
  n[0] = 0;
  strcat(&n[0], "assets/");
  strcat(&n[0], name);

  #if defined(RETRO_WINDOWS)
    U32 resourceSize = 0;
    void* resourceData = Retro_Resource_Load(name, &resourceSize);
    lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
  #elif defined(RETRO_BROWSER)
    RETRO_MAKE_BROWSER_PATH(name);
    lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
  #endif

  assert(imageData);

  //Colour lastColour = Colour_Make(0xCA, 0xFE, 0xBE);
  
  for(U32 i=0;i < width * height * 3;i+=3)
  {
    Retro_Colour colour;
    colour.r = imageData[i + 0];
    colour.g = imageData[i + 1];
    colour.b = imageData[i + 2];

    if (Retro_Palette_Has(colour) == false)
    {
      Retro_Palette_Add(colour);
    }

  }
}

RetroP_Bitmap* RetroP_Bitmap_GetFree()
{
  for (U32 i=0;i < RETRO_DEFAULT_MAX_ANIMATIONS;i++)
  {
    if (RetroCtx->bitmaps[i].bitmapHandle == 0xFFFF)
    {
      RetroCtx->bitmaps[i].bitmapHandle = i;
      return &RetroCtx->bitmaps[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Bitmap* RetroP_Bitmap_Get(Retro_AnimationHandle handle)
{
  return &RetroCtx->bitmaps[handle];
} 

Retro_BitmapHandle Retro_Resources_LoadBitmap(const char* name,  U8 transparentIndex)
{
  RetroP_Bitmap* bitmap = RetroP_Bitmap_GetFree();

  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Retro_Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);

  SDL_Texture* texture = SDL_CreateTexture(RetroPRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  void* pixelsVoid;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;

  Retro_Palette* palette = &RetroCtx->palette;
  
  for(U32 i=0, j=0;i < (width * height * 3);i+=3, j+=4)
  {
    Retro_Colour col;
    col.r = imageData[i+0];
    col.g = imageData[i+1];
    col.b = imageData[i+2];

    int bestIndex = 0x100;
    int bestDistance = 10000000;

    // Match nearest colour by using a treating the two colours as vectors, and matching against the closest distance between the two.
    for (U32 k=0;k < palette->count;k++)
    {
      Retro_Colour pal = palette->colours[k];

      int distance = ((col.r - pal.r) * (col.r - pal.r)) + 
                     ((col.g - pal.g) * (col.g - pal.g)) + 
                     ((col.b - pal.b) * (col.b - pal.b));
      
      if (distance < bestDistance)
      {
        bestDistance = distance;
        bestIndex = k;
      }
    }

    if (bestIndex == 0x100)
      bestIndex = palette->fallback;
    
    Retro_Colour bestColour = palette->colours[bestIndex];

    if (bestIndex == transparentIndex)
      bestColour.a = 0.0f;
    else
      bestColour.a = 255.0f;

    pixels[j+0] = bestColour.a; 
    pixels[j+1] = bestColour.b;
    pixels[j+2] = bestColour.g;
    pixels[j+3] = bestColour.r;
  }

  SDL_UnlockTexture(texture);

  bitmap->w = width;
  bitmap->h = height;
  bitmap->texture = texture;
  bitmap->imageData = imageData;

  return bitmap->bitmapHandle;
}

RetroP_Sprite* Retro_SpriteHandle_GetFree()
{
  for (U32 i=0;i < RETRO_DEFAULT_MAX_SPRITES;i++)
  {
    if (RetroCtx->sprites[i].spriteHandle == 0xFFFF)
    {
      RetroCtx->sprites[i].spriteHandle = i;
      return &RetroCtx->sprites[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Sprite* RetroP_SpriteHandle_Get(Retro_SpriteHandle handle)
{
  return &RetroCtx->sprites[handle];
} 

Retro_SpriteHandle Retro_Sprites_LoadSprite(Retro_BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h)
{
  assert(bitmap);

  RetroP_Sprite* sprite = Retro_SpriteHandle_GetFree();

  sprite->bitmap = bitmap;
  sprite->rect.x = x;
  sprite->rect.y = y;
  sprite->rect.w = w;
  sprite->rect.h = h;

  return sprite->spriteHandle;
}

RetroP_Animation* RetroP_AnimationHandle_GetFree()
{
  for (U32 i=0;i < RETRO_DEFAULT_MAX_ANIMATIONS;i++)
  {
    if (RetroCtx->animations[i].animationHandle == 0xFFFF)
    {
      RetroCtx->animations[i].animationHandle = i;
      return &RetroCtx->animations[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Animation* Retro_AnimationHandle_Get(Retro_AnimationHandle handle)
{
  return &RetroCtx->animations[handle];
} 

Retro_AnimationHandle Retro_Animation_Load(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight, S32 frameOffsetX, S32 frameOffsetY)
{
  assert(numFrames < RETRO_DEFAULT_MAX_ANIMATED_SPRITE_FRAMES);
  
  RetroP_Animation* animation = RetroP_AnimationHandle_GetFree();

  animation->bitmap = bitmap;
  animation->frameCount = numFrames;
  animation->frameLength = frameLengthMilliseconds;
  animation->w = frameWidth;
  animation->h = frameHeight;

  Retro_Rect frame;
  frame.x = originX;
  frame.y = originY;
  frame.w = frameWidth;
  frame.h = frameHeight;

  for(U8 i=0;i < numFrames;i++)
  {
    animation->frames[i] = frame;
    frame.x += frameOffsetX;
    frame.y += frameOffsetY;
  }

  return animation->animationHandle;
}

Retro_AnimationHandle  Retro_Sprites_LoadAnimationH(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  return Retro_Animation_Load(bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, frameWidth, 0);
}

Retro_AnimationHandle  Retro_Sprites_LoadAnimationV(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  return Retro_Animation_Load(bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, 0, frameHeight);
}

S32 Retro_Canvas_Width()
{
  return RetroCtx->settings.canvasWidth;
}

S32 Retro_Canvas_Height()
{
  return RetroCtx->settings.canvasHeight;
}

void Retro_Canvas_Use(U8 id)
{
  assert(id < RetroCtx->settings.canvasCount);
  RetroCtx->canvasTexture = RetroCtx->canvas[id].texture;
  SDL_SetRenderTarget(RetroPRenderer, RetroCtx->canvasTexture);
}

void Retro_Canvas_Clear()
{
  SDL_RenderClear(RetroPRenderer);
}

void Retro_Canvas_Flags(U8 id, U8 flags, U8 colour)
{
  assert(id < RetroCtx->settings.canvasCount);

  RetroP_Canvas* canvas = &RetroCtx->canvas[id];

  canvas->flags = flags;
  canvas->backgroundColour = colour;

  if (flags & CNF_Blend)
    SDL_SetTextureBlendMode(canvas->texture, SDL_BLENDMODE_BLEND);
  else
    SDL_SetTextureBlendMode(canvas->texture, SDL_BLENDMODE_NONE);

}

void  Retro_Canvas_Copy(Retro_BitmapHandle bitmap, Retro_Rect* dstRectangle, Retro_Rect* srcRectangle, U8 copyFlags)
{
  SDL_Rect d, s;

  if (dstRectangle == NULL)
  {
    d.x = 0;
    d.y = 0;
    d.w = RetroCtx->settings.canvasWidth;
    d.h = RetroCtx->settings.canvasHeight;
  }
  else
  {
    d.x = dstRectangle->x;
    d.y = dstRectangle->y;
    d.w = dstRectangle->w;
    d.h = dstRectangle->h;
  }

  if (srcRectangle == NULL)
  {
    s.x = 0;
    s.y = 0;
    s.w = RetroCtx->settings.canvasWidth;
    s.h = RetroCtx->settings.canvasHeight;
  }
  else
  {
    s.x = srcRectangle->x;
    s.y = srcRectangle->y;
    s.w = srcRectangle->w;
    s.h = srcRectangle->h;
  }

  RetroP_Bitmap* bitmapObject = RetroP_Bitmap_Get(bitmap);
  assert(bitmapObject);

  SDL_Texture* texture = (SDL_Texture*) bitmapObject->texture;

  if (copyFlags == 0)
    SDL_RenderCopy(RetroPRenderer, texture, &s, &d);
  else
    SDL_RenderCopyEx(RetroPRenderer, texture, &s, &d, 0.0f, NULL, (SDL_RendererFlip) copyFlags);
}

void  Retro_Canvas_Copy2(Retro_BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags)
{
  assert(bitmap);

  SDL_Rect d, s;

  d.x = dstX;
  d.y = dstY;
  d.w = w;
  d.h = h;

  s.x = srcX;
  s.y = srcY;
  s.w = w;
  s.h = h;

  RetroP_Bitmap* bitmapObject = RetroP_Bitmap_Get(bitmap);
  assert(bitmapObject);

  SDL_Texture* texture = (SDL_Texture*) bitmapObject->texture;

  if (copyFlags)
    SDL_RenderCopy(RetroPRenderer, texture, &s, &d);
  else
    SDL_RenderCopyEx(RetroPRenderer, texture, &s, &d, 0.0f, NULL, (SDL_RendererFlip) copyFlags);
}

void Retro_Canvas_Sprite(Retro_SpriteObject* spriteObject)
{
  assert(spriteObject);
  RetroP_Sprite* sprite = RetroP_SpriteHandle_Get(spriteObject->spriteHandle);
  assert(sprite);

  Retro_Rect d;
  d.x = spriteObject->x;
  d.y = spriteObject->y;
  d.w = sprite->rect.w;
  d.h = sprite->rect.h;

  Retro_Rect s;
  s.x = sprite->rect.x;
  s.y = sprite->rect.y;
  s.w = sprite->rect.w;
  s.h = sprite->rect.h;

  Retro_Canvas_Copy(sprite->bitmap, &d, &s, spriteObject->flags);
}

void  Retro_Canvas_DrawSprite(Retro_SpriteHandle spriteHandle, S32 x, S32 y, U8 copyFlags)
{
  RetroP_Sprite* sprite = RetroP_SpriteHandle_Get(spriteHandle);
  assert(sprite);

  Retro_Rect d;
  d.x = x;
  d.y = y;
  d.w = sprite->rect.w;
  d.h = sprite->rect.h;

  Retro_Rect s;
  s.x = sprite->rect.x;
  s.y = sprite->rect.y;
  s.w = sprite->rect.w;
  s.h = sprite->rect.h;

  Retro_Canvas_Copy(sprite->bitmap, &d, &s, copyFlags);
}

void  Retro_Canvas_Animate(Retro_AnimationObject* animationObject, bool updateTiming)
{
  RetroP_Animation* animation = Retro_AnimationHandle_Get(animationObject->animationHandle);
  assert(animation);

  if (updateTiming && (animationObject->flags & SOF_Animation) != 0)
  {
    animationObject->frameTime += RetroCtx->deltaTime;

    if (animationObject->frameTime >= 1000)
      animationObject->frameTime = 0; // Prevent spiral out of control.


    while(animationObject->frameTime > animation->frameLength)
    {
      animationObject->frameNumber++;
      animationObject->frameTime -= animation->frameLength;

      if (animationObject->frameNumber >= animation->frameCount)
      {
        if (animationObject->flags & SOF_AnimationOnce)
        {
          animationObject->flags &= ~SOF_Animation;
          animationObject->frameNumber = animation->frameCount - 1; // Stop
          break;
        }
        else
        {
          animationObject->frameNumber = 0; // Loop around.
        }
      }
    }
  }

  assert(animationObject->frameNumber < animation->frameCount);

  Retro_Canvas_DrawAnimation(animationObject->animationHandle, animationObject->x, animationObject->y, animationObject->frameNumber, animationObject->flags);
}

void  Retro_Canvas_DrawAnimation(Retro_AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags)
{
  RetroP_Animation* animation = Retro_AnimationHandle_Get(animationHandle);
  assert(animation);
  assert(frame < animation->frameCount);

  Retro_Rect src = animation->frames[frame];
  Retro_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = src.w;
  dst.h = src.h;

  Retro_Canvas_Copy(animation->bitmap, &dst, &src, copyFlags);
}

void Retro_Canvas_Flip()
{
  SDL_RenderPresent(RetroPRenderer);
}

void  Retro_Palette_MakeImpl(Retro_Palette* palette)
{
  assert(palette);
  palette->count = 0;
  palette->fallback = 1;
  palette->transparent = 0;
}

void  Retro_Palette_AddImpl(Retro_Palette* palette, Retro_Colour colour)
{
  assert(palette);
  assert(palette->count <= 255);
  palette->colours[palette->count] = colour;
  ++palette->count;
}

void  Retro_Palette_Add(Retro_Colour colour)
{
  Retro_Palette_AddImpl(&RetroCtx->palette, colour);
}

void  Retro_Palette_AddRGB(U8 r, U8 g, U8 b)
{
  Retro_Colour colour;
  colour.r = r;
  colour.g = g;
  colour.b = b;
  Retro_Palette_AddImpl(&RetroCtx->palette, colour);
}

U8 Retro_Palette_IndexImpl(Retro_Palette* palette, Retro_Colour colour)
{
  assert(palette);

  for(int i=0;i < palette->count;++i)
  {
    Retro_Colour t = palette->colours[i];
    if (t.r == colour.r && t.g == colour.g && t.b == colour.b)
      return i;
  }
  return palette->fallback;
}

U8 Retro_Palette_Index(Retro_Colour colour)
{
  return Retro_Palette_IndexImpl(&RetroCtx->palette, colour);
}

bool Retro_Palette_HasImpl(Retro_Palette* palette, Retro_Colour colour)
{
  assert(palette);

  for(int i=0;i < palette->count;++i)
  {
    Retro_Colour t = palette->colours[i];
    if (t.r == colour.r && t.g == colour.g && t.b == colour.b)
      return true;
  }
  return false;
}

bool Retro_Palette_Has(Retro_Colour colour)
{
  return Retro_Palette_HasImpl(&RetroCtx->palette, colour);
}

Retro_Colour Retro_Palette_Get(U8 index)
{
  Retro_Palette* palette = &RetroCtx->palette;
  return ((palette)->colours[index >= (palette)->count ? (palette)->fallback : index]);
}

void Retro_Palette_Set(U8 index, Retro_Colour colour)
{
  Retro_Palette* palette = &RetroCtx->palette;
  palette->colours[index] = colour;
  palette->count = Retro_Max(palette->count, (index + 1));
}

void Retro_Palette_SetRGB(U8 index, U8 r, U8 g, U8 b)
{
  Retro_Palette* palette = &RetroCtx->palette;
  palette->colours[index] = Retro_Colour_Make(r, g, b);
  palette->count = Retro_Max(palette->count, (index + 1));
}

void Retro_Palette_Copy(const Retro_Palette* src, Retro_Palette* dst)
{
  assert(src);
  assert(dst);

  dst->count = src->count;
  dst->fallback = src->fallback;
  dst->transparent = src->transparent;
  memcpy(dst->colours, src->colours, sizeof(src->colours));
}

U8     Retro_Hex_Char(char c)
{
  c = tolower(c);
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 0;
}

U8     Retro_Hex_U8(const char* str)
{
  char a = str[0], b = str[1];
  
  if (b == '\0')
    return 0;
  else if (a == '\0')
    return Retro_Hex_Char(a);
  else
    return (Retro_Hex_Char(a) << 4) | Retro_Hex_Char(b);
}

S8     Retro_Hex_S8(const char* str)
{
  char a = str[0], b = str[1];

  if (a == '\0')
    return 0;
  else if (b == '\0')
    return Retro_Hex_Char(a);
  else
    return Retro_Hex_Char(a) + Retro_Hex_Char(b) * 16;
}

U16    Retro_Hex_U16(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Retro_Hex_Char(*str);
  }
  return v;
}

S16    Retro_Hex_S16(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Retro_Hex_Char(*str);
  }
  return v;
}

U32    Retro_Hex_U32(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Retro_Hex_Char(*str);
  }
  return v;
}

S32    Retro_Hex_S32(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Retro_Hex_Char(*str);
  }
  return v;
}

Retro_Colour Retro_Colour_Make(U8 r, U8 g, U8 b)
{
  Retro_Colour c;
  c.r = r;
  c.g = g;
  c.b = b;
  return c;
}

Retro_Colour Retro_Hex_Colour(const char* str)
{
  Retro_Colour c;
  c.r = 0;
  c.g = 0;
  c.b = 0;

  if (str == NULL)  // 0 => 0, 0, 0
    return c;

  int n = strlen(str);

  if (n == 1)       // R => Rr, Gg, Bb
  {
    U8 x = Retro_Hex_Char(str[0]) * 16;
    c.r = x;
    c.g = x;
    c.b = x;
  }
  else if (n == 2)  // Rr = Rr, Rr, Rr
  {
    U8 x = Retro_Hex_U8(str);
    c.r = x;
    c.g = x;
    c.b = x;
  }
  else if (n == 3)  // RGB = Rr, Gg, Bb
  {
    c.r = Retro_Hex_Char(str[0]);
    c.g = Retro_Hex_Char(str[1]);
    c.b = Retro_Hex_Char(str[2]);
  }
  else if (n == 4)  // RrGg =  0, 0, 0
  {
  }
  else if (n == 5)  // RrGgB = 0, 0, 0
  {
  }
  else if (n == 6)  // RrGgBb = Rr, Gg, Bb
  {
    c.r = Retro_Hex_U8(str);
    c.g = Retro_Hex_U8(str + 2);
    c.b = Retro_Hex_U8(str + 4);
  }
  else              // ...RrGgBb = Rr, Gg, Bb
  {
    c.r = Retro_Hex_U8(str + n - 6);
    c.g = Retro_Hex_U8(str + n - 4);
    c.b = Retro_Hex_U8(str + n - 2);
  }

  return c;
}

Retro_Point Retro_Point_Make(S32 x, S32 y)
{
  Retro_Point p;
  p.x = x;
  p.y = y;
  return p;
}

Retro_Colour Retro_Colour_ReadRGB(U8* p)
{
  Retro_Colour c;
  c.r = *(p + 0);
  c.g = *(p + 1);
  c.b = *(p + 2);
  return c;
}

Retro_Size Retro_Size_Make(U32 w, U32 h)
{
  Retro_Size s;
  s.w = w;
  s.h = h;
  return s;
}

Retro_Rect Retro_Rect_Make(S32 x, S32 y, S32 w, S32 h)
{
  Retro_Rect r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

U8* RetroP_Arena_ObtainImpl(RetroP_Arena* arena, U32 size)
{
  assert(arena->current + size < arena->end); // Ensure can fit.
  U8* mem = arena->current;
  arena->current += size;
  return mem;
}

#define RetroP_Arena_ObtainImplT(ARENA, T) ((T*) RetroP_Arena_ObtainImpl(ARENA, sizeof(T)))
#define RetroP_Arena_ObtainImplArrayT(ARENA, N, T) ((T*) RetroP_Arena_ObtainImpl(ARENA, N * sizeof(T)))

U8* RetroP_Arena_Obtain(U32 size)
{
  return RetroP_Arena_ObtainImpl(&RetroCtx->arena, size);
}

#define RetroP_Arena_ObtainT(T) ((T*) RetroP_Arena_Obtain(sizeof(T)))

void RetroP_Arena_RewindPtr(U8* mem)
{
  assert(mem >= RetroCtx->arena.begin);
  assert(mem <= RetroCtx->arena.current);
  RetroCtx->arena.current = mem;
}

void RetroP_Arena_RewindU32(U32 offset)
{
  assert(offset < RetroCtx->settings.arenaSize);
  RetroCtx->arena.current = RetroCtx->arena.begin + offset;
}

U32 RetroP_Arena_Current()
{
  return RetroCtx->arena.current - RetroCtx->arena.begin;
}

int  RetroP_Arena_PctSize()
{
  U32 used = (RetroCtx->arena.current - RetroCtx->arena.begin);
  float pct = ((float) used / (float) RETRO_DEFAULT_ARENA_SIZE);
  return (int) (pct * 100.0f);
}

typedef struct
{
  U8   header[4];
  U32  size;
  U32  scopeStackIndex;
  U32  musicSamples;
  char musicName[32];
} RetroP_ArenaSave;

void Retro_Arena_Save(const char* filename)
{
  U32 size;
  U8* mem = Retro_Arena_SaveToMemory(&size);
  FILE* f = fopen(filename, "wb");
  fwrite(mem, size, 1, f);
  fclose(f);
  free(mem);
}

void Retro_Arena_Load(const char* filename, bool loadMusic)
{
  FILE* f = fopen(filename, "rb");
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  U8* mem = (U8*) malloc(size);
  fread(mem, size, 1, f);
  fclose(f);

  Retro_Arena_LoadFromMemory(mem, loadMusic);
  free(mem);
}

U8* Retro_SaveToMem(U8* mem, void* obj, U32 size)
{
  memcpy(mem, obj, size);
  mem += size;
  return mem;
}

U8* Retro_Arena_SaveToMemory(U32* outSize)
{
  U32 memSize = sizeof(RetroP_ArenaSave);
  memSize += (RetroCtx->scopeStackIndex + 1) * sizeof(RetroP_ScopeStack);
  memSize += RetroP_Arena_Current();

  *outSize = memSize;

  U8* mem = (U8*) malloc(memSize);
  memset(mem, 0, memSize);

  U8* p = mem;

  RetroP_ArenaSave s;
  memset(&s, 0, sizeof(RetroP_ArenaSave));

  s.header[0] = 'R';
  s.header[1] = 'E';
  s.header[2] = 'T';
  s.header[3] = 'R';

  s.scopeStackIndex = RetroCtx->scopeStackIndex;
  s.size = (RetroCtx->arena.current - RetroCtx->arena.begin);
  
  if (RetroPMusicContext != NULL)
    s.musicSamples = RetroPMusicContext->samples_remaining;
  else
    s.musicSamples = 0;

  p = Retro_SaveToMem(p, &s, sizeof(RetroP_ArenaSave));

  for(U32 i=0;i < (s.scopeStackIndex + 1);i++)
  {
    //fwrite(&RetroCtx->scopeStack[i], sizeof(ScopeStack), 1,);
    p = Retro_SaveToMem(p, &RetroCtx->scopeStack[i], sizeof(RetroP_ScopeStack));
  }

  p = Retro_SaveToMem(p, RetroCtx->arena.begin, s.size);
  return mem;
}

U8* Retro_ReadFromMem(U8* mem, void* obj, U32 size)
{
  memcpy(obj, mem, size);
  mem += size;
  return mem;
}

void Retro_Arena_LoadFromMemory(U8* mem, bool loadMusic)
{
  U8* p = (U8*) mem;

  RetroP_ArenaSave l;

  p = Retro_ReadFromMem(p, &l, sizeof(RetroP_ArenaSave));

  assert(l.header[0] == 'R');
  assert(l.header[1] == 'E');
  assert(l.header[2] == 'T');
  assert(l.header[3] == 'R');
  assert(l.size < RetroCtx->settings.arenaSize);
 
  RetroCtx->scopeStackIndex = l.scopeStackIndex;
  
  for(U32 i=0;i < l.scopeStackIndex + 1;i++)
  {
    //fread(&RetroCtx->scopeStack[i], sizeof(ScopeStack), 1, f);
    p = Retro_ReadFromMem(p, &RetroCtx->scopeStack[i], sizeof(RetroP_ScopeStack));
  }

  p = Retro_ReadFromMem(p, RetroCtx->arena.begin, l.size);

  RetroCtx->arena.current = RetroCtx->arena.begin + l.size;

  if (loadMusic && RetroPMusicContext != NULL)
  {
    // RetroPMusicContext->samples_remaining = l.musicSamples;
    // micromod_set_position(l.musicSamples);
  }

}

void Retro_Scope_Push(int name)
{
  assert(RetroCtx->scopeStackIndex < 256);

  ++RetroCtx->scopeStackIndex;
  RetroP_ScopeStack* scope = &RetroCtx->scopeStack[RetroCtx->scopeStackIndex];

  scope->name = name;
  scope->p = RetroP_Arena_Current();
}

int Retro_Scope_Name()
{
  RetroP_ScopeStack* scope = &RetroCtx->scopeStack[RetroCtx->scopeStackIndex];
  return scope->name;
}

U8* Retro_Scope_Obtain(U32 size)
{
  RetroP_ScopeStack* scope = &RetroCtx->scopeStack[RetroCtx->scopeStackIndex];
  assert(scope->p + size < RetroCtx->settings.arenaSize); // Ensure can fit.
  return RetroP_Arena_Obtain(size);
}

void Retro_Scope_Rewind()
{
  RetroP_ScopeStack* scope = &RetroCtx->scopeStack[RetroCtx->scopeStackIndex];
  RetroP_Arena_RewindU32(scope->p);
}

void Retro_Scope_Pop()
{
  assert(RetroCtx->scopeStackIndex > 0);
  Retro_Scope_Rewind();
  --RetroCtx->scopeStackIndex;
}

void Retro_Canvas_DrawPalette(S32 Y)
{
  Retro_Canvas_DrawPalette2(Y, 0, RetroCtx->palette.count - 1);
}

void Retro_Canvas_DrawPalette2(S32 Y, U8 from, U8 to)
{
  if (from > to)
  {
    U8 t = to;
    to = from;
    from = t;
  }

  Retro_Rect rect;
  rect.x = 0;
  rect.y = Y;
  rect.w = RetroCtx->settings.canvasHeight / 16;
  rect.h = 8;

  for(U16 i=from, j=1;i < (to + 1);i++, j++)
  {

    Retro_Canvas_DrawRectangle(i, rect);

    if (j % 16 == 0)
    {
      rect.x = 0;
      rect.y += rect.h;
    }
    else
    {
      rect.x += rect.w;
    }
  }

}

void Retro_Canvas_DrawBox(U8 colour, Retro_Rect rect)
{
  Retro_Colour rgb = Retro_Palette_Get(colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  RETRO_SDL_DRAW_PUSH_RGB(t, rgb);

  SDL_RenderDrawRect(RetroPRenderer, &dst);

  RETRO_SDL_DRAW_POP_RGB(t);
}

void Retro_Canvas_DrawRectangle(U8 colour, Retro_Rect rect)
{
  Retro_Colour rgb = Retro_Palette_Get(colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  SDL_SetRenderDrawColor(RetroPRenderer, rgb.r, rgb.g, rgb.b, 0xFF);
  SDL_RenderFillRect(RetroPRenderer, &dst);
  SDL_SetRenderDrawColor(RetroPRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

char* gFmtScratch;

void  Retro_Canvas_MonoPrint(S32 x, S32 y, S32 w, Retro_Font* font, U8 colour, const char* str)
{
  assert(font);
  assert(str);

  Retro_Colour rgb = Retro_Palette_Get(colour);

  SDL_Rect s, d;
  s.x = 0;
  s.y = 0;
  s.w = 0;
  s.h = font->height;
  d.x = x;
  d.y = y;
  d.w = 0;
  d.h = s.h; 

  RetroP_Bitmap* bitmapObject = RetroP_Bitmap_Get(font->bitmap);
  assert(bitmapObject);

  SDL_Texture* texture = (SDL_Texture*) bitmapObject->texture;

  RETRO_SDL_TEXTURE_PUSH_RGB(t, texture, rgb);

  while(true)
  {
    U8 c = *str++;

    if (c == 0x0)
      break;

    if (c == ' ')
    {
      d.x += w;
      continue;
    }

    s.x = font->x[c];
    s.w = font->widths[c];

    S32 ox = d.x;
    S32 ix = (w - font->widths[c]);

    d.x += ix / 2;
    d.w = s.w;

    SDL_RenderCopy(RetroPRenderer, texture, &s, &d);

    d.x = ox + w;
  }

  RETRO_SDL_TEXTURE_POP_RGB(t, texture);
}

void Retro_Canvas_Print(S32 x, S32 y, Retro_Font* font, U8 colour, const char* str)
{
  assert(font);
  assert(str);

  Retro_Colour rgb = Retro_Palette_Get(colour);

  SDL_Rect s, d;
  s.x = 0;
  s.y = 0;
  s.w = 0;
  s.h = font->height;
  d.x = x;
  d.y = y;
  d.w = 0;
  d.h = s.h; 

  RetroP_Bitmap* bitmapObject = RetroP_Bitmap_Get(font->bitmap);
  assert(bitmapObject);

  SDL_Texture* texture = (SDL_Texture*) bitmapObject->texture;

  RETRO_SDL_TEXTURE_PUSH_RGB(t, texture, rgb);

  while(true)
  {
    U8 c = *str++;

    if (c == 0x0)
      break;
    
    if (c == ' ')
    {
      d.x += font->widths[' '];
      continue;
    }

    s.x = font->x[c];
    s.w = font->widths[c];
    d.w = s.w;

    SDL_RenderCopy(RetroPRenderer, texture, &s, &d);

    d.x += d.w;
  }

  RETRO_SDL_TEXTURE_POP_RGB(t, texture);

}

void Retro_Canvas_Printf(S32 x, S32 y, Retro_Font* font, U8 colour, const char* fmt, ...)
{
  assert(font);
  assert(fmt);
  va_list args;
  va_start(args, fmt);
  vsprintf(gFmtScratch, fmt, args);
  va_end(args);

  Retro_Canvas_Print(x, y, font, colour, gFmtScratch);
}

void Retro_Canvas_Presentation(Retro_CanvasPresentation presentation, float alpha, float beta)
{
  RetroCtx->framePresentation = presentation;
  RetroCtx->frameAlpha = alpha;
  RetroCtx->frameBeta = beta;
}

void Retro_Sprites_NewAnimationObject(Retro_AnimationObject* inAnimatedSpriteObject, Retro_AnimationHandle animation, S32 x, S32 y)
{
  assert(inAnimatedSpriteObject);

  inAnimatedSpriteObject->animationHandle = animation;
  inAnimatedSpriteObject->flags = 0;
  inAnimatedSpriteObject->frameNumber = 0;
  inAnimatedSpriteObject->frameTime = 0;
  inAnimatedSpriteObject->x = x;
  inAnimatedSpriteObject->y = y;
}

void Retro_Sprites_PlayAnimationObject(Retro_AnimationObject* inAnimatedSpriteObject, bool playing, bool loop)
{
  assert(inAnimatedSpriteObject);

  if (playing)
    inAnimatedSpriteObject->flags |= SOF_Animation;
  else
    inAnimatedSpriteObject->flags &= ~SOF_Animation;

  if (!loop)
    inAnimatedSpriteObject->flags |= SOF_AnimationOnce;
  else
    inAnimatedSpriteObject->flags &= ~SOF_AnimationOnce;

}

void Retro_Sprites_SetAnimationObject(Retro_AnimationObject* animatedSpriteObject, Retro_AnimationHandle newAnimation, bool animate)
{
  assert(animatedSpriteObject);

  animatedSpriteObject->animationHandle = newAnimation;
  if (animate)
    animatedSpriteObject->flags |= SOF_Animation;
  else
    animatedSpriteObject->flags &= ~SOF_Animation;
  animatedSpriteObject->frameNumber = 0;
}

void  Retro_Debug(Retro_Font* font)
{
  assert(font);
  RetroFourByteUnion f;
  f.q = Retro_Scope_Name();
  
  U32 soundObjectCount = 0;
  for(U32 i=0;i < RETRO_DEFAULT_MAX_SOUND_OBJECTS;i++)
  {
    RetroP_SoundObject* soundObj = &RetroCtx->soundObject[i];

    if (soundObj->sound != NULL)
      soundObjectCount++;
  }

  int music = -1;

  if (RetroPMusicContext != NULL)
  {
    music = (int) 100 - (((float) RetroPMusicContext->samples_remaining / (float) RetroPMusicContext->length) *100.0f);
  }

  Retro_Canvas_Printf(0, RetroCtx->settings.canvasHeight - font->height, font, RetroCtx->palette.fallback, "Ctx=%i, Scope=%c%c%c%c Mem=%i%% FPS=%.2g Dt=%i Snd=%i, Mus=%i", RetroCtx->id, f.b[3], f.b[2], f.b[1], f.b[0], RetroP_Arena_PctSize(), RetroCtx->fps, RetroCtx->deltaTime, soundObjectCount, music);
}

RetroP_Sound* RetroP_Sound_GetFree()
{
  for (U32 i=0;i < RETRO_DEFAULT_MAX_SOUNDS;i++)
  {
    if (RetroCtx->sounds[i].soundHandle == 0xFFFF)
    {
      RetroCtx->sounds[i].soundHandle = i;
      return &RetroCtx->sounds[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Sound* RetroP_Sound_Get(Retro_AnimationHandle handle)
{
  return &RetroCtx->sounds[handle];
} 

Retro_SoundHandle Retro_Resources_LoadSound(const char* name)
{
  RetroP_Sound* sound = RetroP_Sound_GetFree();

  #ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resource = Retro_Resource_Load(name, &resourceSize);
  SDL_LoadWAV_RW(SDL_RWFromConstMem(resource, resourceSize), 0, &sound->spec, &sound->buffer, &sound->length);
  #else
  RETRO_MAKE_BROWSER_PATH(name);
  SDL_LoadWAV(RETRO_BROWSER_PATH, &sound->spec, &sound->buffer, &sound->length);
  #endif

  if (sound->spec.format != RetroPAudioDevice.specification.format || sound->spec.freq != RetroPAudioDevice.specification.freq || sound->spec.channels != RetroPAudioDevice.specification.channels)
  {
    // Do a conversion
    SDL_AudioCVT cvt;
    int e = SDL_BuildAudioCVT(&cvt, sound->spec.format, sound->spec.channels, sound->spec.freq, RetroPAudioDevice.specification.format, RetroPAudioDevice.specification.channels, RetroPAudioDevice.specification.freq);
    assert(e >= 0);

    cvt.buf = malloc(sound->length * cvt.len_mult);
    memcpy(cvt.buf, sound->buffer, sound->length);
    cvt.len = sound->length;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(sound->buffer);

    sound->buffer = cvt.buf;
    sound->length = cvt.len_cvt;
    sound->spec = RetroPAudioDevice.specification;

    // printf("Loaded Audio %s but had to convert it into a internal format.\n", name);
  }
  else
  {
   // printf("Loaded Audio %s\n", name);
  }

  return sound->soundHandle;

}

void  Retro_Audio_PlaySound(Retro_SoundHandle soundHandle, U8 volume)
{
  RetroP_Sound* sound = RetroP_Sound_Get(soundHandle);

  for(U32 i=0;i < RETRO_DEFAULT_MAX_SOUND_OBJECTS;i++)
  {
    RetroP_SoundObject* soundObj = &RetroCtx->soundObject[i];
    if (soundObj->sound != NULL)
     continue;

    soundObj->sound = sound;
    soundObj->p = 0;
    soundObj->volume = volume > SDL_MIX_MAXVOLUME ? SDL_MIX_MAXVOLUME : volume;

    return;
  }
}

void  Retro_Audio_ClearSounds()
{
  for(U32 i=0;i < RETRO_DEFAULT_MAX_SOUND_OBJECTS;i++)
  {
    RetroP_SoundObject* soundObj = &RetroCtx->soundObject[i];

    soundObj->sound = NULL;
    soundObj->p = 0;
    soundObj->volume = 0;
  }
}

void Retro_Audio_PlayMusic(const char* name)
{
  if (RetroPMusicContext != NULL)
  {
    Retro_Audio_StopMusic();
  }

  RetroPMusicContext = malloc(sizeof(micromod_sdl_context));
  memset(RetroPMusicContext, 0, sizeof(micromod_sdl_context));

  void* data = NULL;
  U32 dataLength = 0;

#ifdef RETRO_WINDOWS
  data = Retro_Resource_Load(name, &dataLength);
#endif

#ifdef RETRO_BROWSER
  RETRO_MAKE_BROWSER_PATH(name);
  FILE* f = fopen(RETRO_BROWSER_PATH, "rb");
  fseek(f, 0, SEEK_END);
  dataLength = ftell(f);
  fseek(f, 0, SEEK_SET);

  RetroCtx->musicFileData = malloc(dataLength);
  fread(RetroCtx->musicFileData, dataLength, 1, f);
  fclose(f);

  data = RetroCtx->musicFileData;
#endif

  micromod_initialise(data, SAMPLING_FREQ * OVERSAMPLE);
  RetroPMusicContext->samples_remaining = micromod_calculate_song_duration();
  RetroPMusicContext->length = RetroPMusicContext->samples_remaining;

}

void Retro_Audio_StopMusic()
{
  if (RetroPMusicContext == NULL)
  {
    return;
  }

  #if defined(RETRO_BROWSER)
    free(RetroCtx->musicFileData);
    RetroCtx->musicFileData = NULL;
  #endif

  free(RetroPMusicContext);
  RetroPMusicContext = NULL;
}

void Retro_SDL_SoundCallback(void* userdata, U8* stream, int streamLength)
{
  if (RetroAudioCtx == NULL)
    return;

  SDL_memset(stream, 0, streamLength);

  if (RetroPMusicContext != NULL)
  {

    // int uSize = (RetroAudioCtx->soundDevice.specification.format == AUDIO_S16 ? sizeof(short) : sizeof(float));

    long count = 0;

    if (RetroPAudioDevice.specification.format == AUDIO_S16)
      count = streamLength / 2;
    else
      count = streamLength / 4;
    
    if( RetroPMusicContext->samples_remaining < count ) {
      /* Clear output.*/
      count = RetroPMusicContext->samples_remaining;
    }

    if( count > 0 ) {
      /* Get audio from replay.*/

      memset( RetroPMusicContext->mix_buffer, 0, count * NUM_CHANNELS * sizeof( short ) );
      micromod_get_audio( RetroPMusicContext->mix_buffer, count );
      
      if (RetroPAudioDevice.specification.format == AUDIO_S16)
        micromod_sdl_downsample( RetroPMusicContext, RetroPMusicContext->mix_buffer, (short *) stream, count );
      else
        micromod_sdl_downsample_float( RetroPMusicContext, RetroPMusicContext->mix_buffer, (float*) stream, count);
      
      RetroPMusicContext->samples_remaining -= count;
    }
    else
    {
      RetroPMusicContext->samples_remaining = RetroPMusicContext->length;
    }
  }

  for(U32 i=0;i < RETRO_DEFAULT_MAX_SOUND_OBJECTS;i++)
  {
    RetroP_SoundObject* soundObj = &RetroAudioCtx->soundObject[i];

    if (soundObj->sound == NULL)
      continue;

    U32 soundLength = soundObj->sound->length;
    
    U32 mixLength = ( (U32) streamLength > soundLength ? soundLength : streamLength);
    

    if (soundObj->p + mixLength >= soundLength)
    {
      mixLength = soundLength - soundObj->p;
    }

    
    SDL_MixAudioFormat(stream, soundObj->sound->buffer + soundObj->p, soundObj->sound->spec.format, mixLength, SDL_MIX_MAXVOLUME / 2);

    soundObj->p += mixLength;

    if (soundObj->p >= soundObj->sound->length)
    {
      // Finished
      soundObj->sound = NULL;
      soundObj->p = 0;
      soundObj->volume = 0;
    }
  }
}

void  Retro_Font_Make(Retro_Font* font)
{
  assert(font);
  memset(font->widths, 0, sizeof(font->widths));
  font->height = 0;
  font->bitmap = 0xFFFF;
}

void Retro_Resources_LoadFont(const char* name, Retro_Font* outFont, Retro_Colour markerColour, Retro_Colour transparentColour)
{
  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Retro_Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);

  SDL_Texture* texture = SDL_CreateTexture(RetroPRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height - 1);

  void* pixelsVoid;
  int pitch;

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;
  
  U32 i, j;

  U32 lx = 0xCAFEBEEF;
  U8  ch = '!';

  // Scan the first line for markers.
  for(i=0;i < width * 3;i+=3)
  {
    Retro_Colour col = Retro_Colour_ReadRGB(&imageData[i]);
    if (Retro_Colour_Equals(col, markerColour))
    {
      int x = i / 3;

      if (lx == 0xCAFEBEEF)
      {
        lx = 0;
      }
      else
      {
        outFont->x[ch] = lx;
        outFont->widths[ch] = x - lx;
        ch++;
        lx = x + 1;
      }
    }
  }

  outFont->widths[' '] = outFont->widths['e'];

  // Copy rest of image into the texture.
  for(i=0, j=width * 3;i < width * height * 4;i+=4, j+=3)
  {
    Retro_Colour col = Retro_Colour_ReadRGB(&imageData[j]);

    pixels[i+0] = 0xFF;
    pixels[i+1] = 0xFF;
    pixels[i+2] = 0xFF;

    if (Retro_Colour_Equals(col, transparentColour))
    {
      pixels[i+3] = 0x00;
    }
    else
    {
      pixels[i+3] = 0xFF;
    }
  }

  SDL_UnlockTexture(texture);

  RetroP_Bitmap* bitmap = RetroP_Bitmap_GetFree();

  bitmap->texture = texture;
  bitmap->w = width;
  bitmap->h = height;
  bitmap->imageData = imageData;
  

  outFont->height = height - 1;
  outFont->bitmap = bitmap->bitmapHandle;

}

void  Retro_Resources_LoadFontFixed(const char* name, Retro_Font* outFont, U8 w, Retro_Colour transparentColour)
{
  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Retro_Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);

  SDL_Texture* texture = SDL_CreateTexture(RetroPRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height - 1);

  void* pixelsVoid;
  int pitch;

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;

  U32 i, j;

  U32 lx = 0xCAFEBEEF;
  U8  ch = '!';
  U16 nbChars = (width / w);

  // Scan the first line for markers.
  for(i=0;i < nbChars;i++)
  {
    outFont->x[ch] = (i * w);
    outFont->widths[ch] = w;
    ch++;
  }

  outFont->widths[' '] = w;

  // Copy rest of image into the texture.
  for(i=0, j=width * 3;i < width * height * 4;i+=4, j+=3)
  {
    Retro_Colour col = Retro_Colour_ReadRGB(&imageData[j]);

    pixels[i+0] = 0xFF;
    pixels[i+1] = 0xFF;
    pixels[i+2] = 0xFF;

    if (Retro_Colour_Equals(col, transparentColour))
    {
      pixels[i+3] = 0x00;
    }
    else
    {
      pixels[i+3] = 0xFF;
    }
  }

  SDL_UnlockTexture(texture);

  RetroP_Bitmap* bitmap = RetroP_Bitmap_GetFree();

  bitmap->texture = texture;
  bitmap->w = width;
  bitmap->h = height;
  bitmap->imageData = imageData;

  outFont->height = height - 1;
  outFont->bitmap = bitmap->bitmapHandle;
}

int Retro_Input_TextInput(char* str, U32 capacity)
{
  assert(str);
  U32 len = strlen(str);

  switch(RetroSharedCtx.inputCharState)
  {
    default:
    case ICS_None:
    return 0;
    case ICS_Character:
    {
      if (len < capacity)
      {
        str[len] = RetroSharedCtx.inputChar;
        return 1;
      }
    }
    return 0;
    case ICS_Backspace:
    {
      if (len > 0)
      {
        str[len-1] = 0;
        return 1;
      }
    }
    return 0;
    case ICS_Enter:
      if (len > 0)
        return 2;
    return 0;
  }
  return 0;
}

RetroP_InputActionBinding* Retro_Input_GetAction(int action)
{
  for(int i=0;i < RETRO_DEFAULT_MAX_INPUT_ACTIONS;++i)
  {
    RetroP_InputActionBinding* binding = &RetroCtx->inputActions[i];
    if (binding->action == action)
      return binding;
  }
  return NULL;
}

RetroP_InputActionBinding* Retro_Input_MakeAction(int action)
{
  for(int i=0;i < RETRO_DEFAULT_MAX_INPUT_ACTIONS;++i)
  {
    RetroP_InputActionBinding* binding = &RetroCtx->inputActions[i];
    if (binding->action == 0xDEADBEEF)
    {
      binding->action = action;
      return binding;
    }
  }
  assert(true); // Ran out of InputActionBindings
  return NULL;
}


void  Retro_Input_BindKey(int key, int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  
  if (binding == NULL)
  {
    binding = Retro_Input_MakeAction(action);
  }

  for (U32 i=0;i < RETRO_DEFAULT_MAX_INPUT_BINDINGS;i++)
  {
    if (binding->keys[i] == 0)
    {
      binding->keys[i] = key;
      return;
    }
  }

  assert(true); // To many keys to bound
}

void  Retro_Input_BindAxis(int axis, int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);

  if (binding == NULL)
  {
    binding = Retro_Input_MakeAction(action);
  }

  for (U32 i=0;i < RETRO_DEFAULT_MAX_INPUT_BINDINGS;i++)
  {
    if (binding->axis[i] == 0)
    {
      binding->axis[i] = axis;
      return;
    }
  }

  assert(true); // To many keys to bound
}

bool  Retro_Input_Down(int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  assert(binding);

  return binding->state == 1;
}

bool  Retro_Input_Released(int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  assert(binding);

  return binding->state == 0 && binding->lastState == 1;
}

bool  Retro_Input_Pressed(int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  assert(binding);

  return binding->state == 1 && binding->lastState == 0;
}

S16   Retro_Input_Axis(int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  assert(binding);

  return binding->state;
}

S16   Retro_Input_DeltaAxis(int action)
{
  RetroP_InputActionBinding* binding = Retro_Input_GetAction(action);
  assert(binding);

  return binding->state - binding->lastState;
}

typedef enum
{
  TF_None    = 0,
  TF_Started = 1,
  TF_Paused  = 2
} Retro_TimerFlags;

void  Retro_Timer_Make(Retro_Timer* timer)
{
  assert(timer);
  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Retro_Timer_Start(Retro_Timer* timer)
{
  assert(timer);
  timer->flags = TF_Started;
  timer->start = SDL_GetTicks();
  timer->paused = 0;
}

void  Retro_Timer_Stop(Retro_Timer* timer)
{
  assert(timer);

  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Retro_Timer_Pause(Retro_Timer* timer)
{
  assert(timer);

  if (timer->flags == TF_Started)
  {
    timer->flags |= TF_Paused;
    timer->paused = SDL_GetTicks() - timer->start;
    timer->start = 0;
  }
}

void  Retro_Timer_Unpause(Retro_Timer* timer)
{
  assert(timer);

  if (timer->flags == 3 /* Started | Paused */)
  {
    timer->flags = TF_Started; // &= ~Paused
    timer->start = SDL_GetTicks() - timer->paused;
    timer->paused = 0;
  }
}

U32   Retro_Timer_Ticks(Retro_Timer* timer)
{
  assert(timer);

  U32 time = 0;

  if (timer->flags != 0) // Started || Paused
  {
    if (timer->flags > TF_Started) // Paused
    {
      time = timer->paused;
    }
    else
    {
      time = SDL_GetTicks() - timer->start;
    }
  }

  return time;
}

bool  Retro_Timer_Started(Retro_Timer* timer)
{
  return timer->flags >= TF_Started;
}

bool  Retro_Timer_Paused(Retro_Timer* timer)
{
  return timer->flags >= TF_Paused;
}

void RetroP_Canvas_Present()
{

  SDL_SetRenderTarget(RetroPRenderer, RetroCtx->mainTexture);


  #if 1

  for (int i=0;i < RetroCtx->settings.canvasCount;i++)
  {
    if (RetroCtx->canvas[i].flags & CNF_Render)
    {
      SDL_RenderCopy(RetroPRenderer, RetroCtx->canvas[i].texture, NULL, NULL);
    }
  }

  #else

  switch(RetroCtx->framePresentation)
  {
    case FP_Normal:
    {
      for (int i=0;i < RetroCtx->settings.canvasCount;i++)
      {
        if (RetroCtx->canvas[i].flags & CNF_Render)
        {
          SDL_RenderCopy(RetroPRenderer, RetroCtx->canvas[i].texture, NULL, NULL);
        }
      }
    }
    break;
    case FP_WaveH:
    {
      U32 accuracy = 2;

      for (U32 u=0;u < RETRO_DEFAULT_WINDOW_HEIGHT;u+=accuracy)
      {
        SDL_Rect src;
        SDL_Rect dst;
        src.x = 0;
        src.y = u;
        src.w = RETRO_DEFAULT_CANVAS_WIDTH;
        src.h = accuracy;

        dst = src;

        float x0 = (float) u / (float) RETRO_DEFAULT_WINDOW_HEIGHT;
        dst.x = (sin((RetroCtx->frameCount % 1000) * RetroCtx->frameAlpha + x0 * 3.14f) * RetroCtx->frameBeta);
        dst.y = u * 2;
        dst.w = RETRO_DEFAULT_WINDOW_WIDTH;
        dst.h = accuracy * 2;

        for (int i=0;i < RETRO_DEFAULT_CANVAS_COUNT;i++)
        {
          if (RetroCtx->canvas[i].flags & CNF_Render)
          {
            SDL_RenderCopy(RetroPRenderer, RetroCtx->canvas[i].texture, &src, &dst);
          }
        }
      }
    }
    break;
    case FP_WaveV:
    {
      U32 accuracy = 2;

      for (U32 u=0;u < RETRO_DEFAULT_WINDOW_HEIGHT;u+=accuracy)
      {
        SDL_Rect src;
        SDL_Rect dst;
        src.x = u;
        src.y = 0;
        src.w = accuracy;
        src.h = RETRO_DEFAULT_CANVAS_HEIGHT;

        dst = src;

        float y0 = (float) u / (float) RETRO_DEFAULT_WINDOW_HEIGHT;
        dst.x = u * 2;
        dst.y = (sin((RetroCtx->frameCount % 1000) * RetroCtx->frameAlpha + y0 * 3.14f) * RetroCtx->frameBeta);
        dst.w = accuracy * 2;
        dst.h = RETRO_DEFAULT_WINDOW_HEIGHT;

        for (int i=0;i < RETRO_DEFAULT_CANVAS_COUNT;i++)
        {
          if (RetroCtx->canvas[i].flags & CNF_Render)
          {
            SDL_RenderCopy(RetroPRenderer, RetroCtx->canvas[i].texture, &src, &dst);
          }
        }
      }
    }
    break;
    case FP_Scale:
    {

      SDL_Rect src;
      SDL_Rect dst;
      src.w = (float) RETRO_DEFAULT_CANVAS_WIDTH * RetroCtx->frameAlpha;
      src.h = (float) RETRO_DEFAULT_CANVAS_HEIGHT * RetroCtx->frameBeta;
      src.x = 0; //src.w / 2;
      src.y = 0; //src.h / 2;

      dst = src;
      dst.x = RETRO_DEFAULT_WINDOW_WIDTH / 2 - (src.w);
      dst.y = RETRO_DEFAULT_WINDOW_HEIGHT / 2 - (src.h);
      dst.w *= 2;
      dst.h *= 2;

      for (int i=0;i < RETRO_DEFAULT_CANVAS_COUNT;i++)
      {
        if (RetroCtx->canvas[i].flags & CNF_Render)
        {
          SDL_RenderCopy(RetroPRenderer, RetroCtx->canvas[i].texture, &src, &dst);
        }
      }
    }
    break;
  }
  #endif
}

void RetroP_Frame()
{
  RetroCtx->deltaTime = Retro_Timer_Ticks(&RetroCtx->deltaTimer);

  RetroCtx->fps = RetroCtx->frameCount / (Retro_Timer_Ticks(&RetroCtx->fpsTimer) / 1000.0f);
  if (RetroCtx->fps > 200000.0f)
  {
    RetroCtx->fps = 0.0f;
  }

  for (U32 i=0;i < RETRO_DEFAULT_MAX_INPUT_ACTIONS;i++)
  {
    RetroP_InputActionBinding* binding = &RetroCtx->inputActions[i];
    if (binding->action == 0xDEADBEEF)
      break;

    binding->lastState = binding->state;
    binding->state = 0;

    for (U32 j=0; j < RETRO_DEFAULT_MAX_INPUT_BINDINGS;j++)
    {
      int key = binding->keys[j];

      if (key == SDL_SCANCODE_UNKNOWN || key >= SDL_NUM_SCANCODES)
        break;

      binding->state |= (RetroSharedCtx.keyState[key] != 0) ? 1 : 0;
    }

    // @TODO Axis
  }
  
  for (U8 i=0;i < RetroCtx->settings.canvasCount;i++)
  {
    if (RetroCtx->canvas[i].flags & CNF_Clear)
    {
      Retro_Canvas_Use(i);
      Retro_Colour col = Retro_Palette_Get(RetroCtx->canvas[i].backgroundColour);
      SDL_SetRenderDrawColor(RetroPRenderer, col.r, col.g, col.b, 0x00);
      Retro_Canvas_Clear();
      SDL_SetRenderDrawColor(RetroPRenderer, 0xFF, 0xFF, 0xFF, 0x00);
    }
  }

  Retro_Canvas_Use(0);

  RetroCtx->library.stepFunction();

  RetroP_Canvas_Present();

  SDL_SetRenderTarget(RetroPRenderer, NULL);

  ++RetroCtx->frameCount;
  
  Retro_Timer_Start(&RetroCtx->deltaTimer);
}

void RetroP_Restart()
{
  RetroCtx->arena.current = RetroCtx->arena.begin;

  RetroCtx->scopeStackIndex = 0;
  RetroCtx->scopeStack[0].p = 0;
  RetroCtx->scopeStack[0].name = 'INIT';

  RetroCtx->library.startFunction();
}

void RetroP_InitialiseRetro(Retro_Settings* settings)
{
  static bool isSetup = false;

  if (isSetup)
    return;
  
  isSetup = true;

  RetroContextCount = 0;
  RetroCtx = NULL;
  RetroAudioCtx = NULL;
  RetroPMinWaitTime = 10000000;
  memset(&RetroContexts, 0, sizeof(RetroP_Context*) * RETRO_MAX_CONTEXT);

  SDL_Init(SDL_INIT_EVERYTHING);

  RetroPWindow = SDL_CreateWindow( 
    settings->caption,
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED,
    settings->windowWidth,
    settings->windowHeight,
    SDL_WINDOW_SHOWN
  );

  RetroPWindowWidth = settings->windowWidth;
  RetroPWindowHeight = settings->windowHeight;

  // Shared Renderer
  RetroPRenderer = SDL_CreateRenderer(RetroPWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  if (RetroPRenderer == NULL)
  {
    printf("[Retro] SDL Renderer init error; %s\n", SDL_GetError());
  }

  assert(RetroPRenderer);

  // Shared Audio Device
  SDL_AudioSpec want, got;
  memset(&want, 0, sizeof(want));
  memset(&got, 0, sizeof(got));

  want.freq     = settings->audioFrequency;
  want.format   = AUDIO_S16;
  want.channels = settings->audioChannels;
  want.samples  = settings->audioSamples;
  want.callback = Retro_SDL_SoundCallback;
  want.userdata = NULL;

  if (SDL_OpenAudio(&want, &got) < 0)
  {
    want.format = AUDIO_F32;
    if (SDL_OpenAudio(&want, &got) < 0)
    {
      printf("Sound Init Error: %s\n", SDL_GetError());
    }
  }

  RetroPAudioDevice.specification = got;
  RetroPMusicContext = NULL;

#ifdef RETRO_BROWSER
  RetroPMusicFileData = NULL;
#endif

  gFmtScratch = malloc(1024);
}

bool RetroP_RetroContextFrame()
{
  bool quit = false;
  U32  count = 0;
  
  SDL_Event event;

  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
      {
        quit = true;
      }
      break;
      case SDL_TEXTINPUT:
      {
        RetroSharedCtx.inputChar = event.text.text[0];
        RetroSharedCtx.inputCharState = ICS_Character;
      }
      break;
      case SDL_KEYDOWN:
      {

        if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
          RetroSharedCtx.inputCharState = ICS_Backspace;
        }
        else if (event.key.keysym.sym == SDLK_RETURN)
        {
          RetroSharedCtx.inputCharState = ICS_Enter;
        }
      }
      break;
    }
  }

  RetroSharedCtx.keyState = SDL_GetKeyboardState(NULL);


  for(U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    RetroCtx = RetroContexts[i];
    if (RetroCtx == NULL)
      continue;
    ++count;

    RetroP_Frame();
    RetroP_Canvas_Present();

    quit |= RetroCtx->quit;
  }
  RetroCtx = NULL;

  SDL_SetRenderTarget(RetroPRenderer, NULL);

  SDL_SetRenderDrawColor(RetroPRenderer, 0x88, 0x00, 0x00, 0xFF);
  SDL_RenderClear(RetroPRenderer);
  SDL_SetRenderDrawColor(RetroPRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

  // @TODO A user defined layout function, which can override this loop.

  S32 windowWidth = 0, windowHeight = 0;
  SDL_GetWindowSize(RetroPWindow, &windowWidth, &windowHeight);

  for(U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    RetroCtx = RetroContexts[i];
    if (RetroCtx == NULL)
      continue;
    
    SDL_Rect d;

    d.w = RetroCtx->settings.canvasWidth   * RetroCtx->settings.canvasScaleX;
    d.h = RetroCtx->settings.canvasHeight  * RetroCtx->settings.canvasScaleY;

    if (RetroCtx->settings.canvasX == 0xFFFFffff)
      d.x = (windowWidth - d.w) / 2;
    else
      d.x = RetroCtx->settings.canvasX;

    if (RetroCtx->settings.canvasY == 0xFFFFffff)
      d.y = (windowHeight - d.h) / 2;
    else
      d.y = RetroCtx->settings.canvasY;

    SDL_RenderCopy(RetroPRenderer, RetroCtx->mainTexture, NULL, &d);
  }

  RetroCtx = NULL;

  Retro_Canvas_Flip();
    

  return quit || (count == 0);
}

void RetroP_RetroLoop()
{
  // TODO:
  //  Cancel out if there is also a RetroLoop running
  //  We just 'merge' them into two.

#ifdef RETRO_WINDOWS
  
  Retro_Timer_Start(&RetroPCapTimer);

  while(true)
  {
    U32 frameTicks = Retro_Timer_Ticks(&RetroPCapTimer);

    if (frameTicks < RetroPMinWaitTime)
    {
      U32 diff = (RetroPMinWaitTime - frameTicks);
      SDL_Delay(diff);
    }

    Retro_Timer_Start(&RetroPCapTimer);

    if (RetroP_RetroContextFrame())
      break;
  }
#endif

#ifdef RETRO_BROWSER
  emscripten_set_main_loop(RetroP_RetroContextFrame, RetroPMinWaitTime, 1);
#endif

}

void Retro_Shutdown()
{
  SDL_Quit();
}

static RetroP_Context* RetroP_AllocateContextAndArenaBytes(Retro_Settings* settings)
{
  static U32 contextId = 0;

  U32 id = contextId++;

  // This is essentially one massive allocation, containing the Context and Resources and the Arena at the end.
  U32 nbBytes = 0;
  nbBytes += sizeof(RetroP_Context);
  nbBytes += settings->maxBitmaps      * sizeof(RetroP_Bitmap);
  nbBytes += settings->maxSounds       * sizeof(RetroP_Sound);
  nbBytes += settings->maxSprites      * sizeof(RetroP_Sprite);
  nbBytes += settings->maxAnimations   * sizeof(RetroP_Animation);
  nbBytes += settings->canvasCount     * sizeof(RetroP_Canvas);
  nbBytes += settings->maxInputActions * sizeof(RetroP_InputActionBinding);
  nbBytes += settings->maxSoundObjects * sizeof(RetroP_SoundObject);
  nbBytes += settings->arenaSize;

  printf("[Retro:%i] Allocating %i bytes, of %i is ROM and %i is RAM\n", id, nbBytes, nbBytes - settings->arenaSize, settings->arenaSize);

  U8* mem = malloc(nbBytes);
  memset(mem, 0, nbBytes);

  RetroP_Context* context = (RetroP_Context*) mem;

  context->mem.begin   = mem;
  context->mem.current = mem + sizeof(RetroP_Context);
  context->mem.end     = mem + nbBytes;

  context->arena.begin   = mem + nbBytes - settings->arenaSize;
  context->arena.current = context->arena.begin;
  context->arena.end     = context->arena.begin + settings->arenaSize;

  context->id = id;
  return context;
}

static RetroP_Context* RetroP_InitContext(Retro_Settings* settings, RetroP_Library* library)
{
  // Allocate memory, and arena. 
  RetroCtx = RetroP_AllocateContextAndArenaBytes(settings);
  
  bool addedContext = false;
  for(U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    if (RetroContexts[i] == NULL)
    {
      addedContext = true;
      RetroContexts[i] = RetroCtx;
      break;
    }
  }
  assert(addedContext);

  memcpy(&RetroCtx->settings, settings, sizeof(Retro_Settings));


  // Copy in context library handle (if any), functions
  memcpy(&RetroCtx->library, library, sizeof(RetroP_Library));


  // Loaded Resources
  RetroCtx->bitmaps       = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxBitmaps,    RetroP_Bitmap);
  for(U32 i=0;i < settings->maxBitmaps;i++)
    RetroCtx->bitmaps[i].bitmapHandle = 0xFFFF;

  RetroCtx->sounds        = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxSounds,     RetroP_Sound);
  for(U32 i=0;i < settings->maxSounds;i++)
    RetroCtx->sounds[i].soundHandle = 0xFFFF;

  RetroCtx->sprites       = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxSprites,    RetroP_Sprite);
  for(U32 i=0;i < settings->maxSprites;i++)
    RetroCtx->sprites[i].spriteHandle = 0xFFFF;

  RetroCtx->animations    = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxAnimations, RetroP_Animation);
  for(U32 i=0;i < settings->maxAnimations;i++)
    RetroCtx->animations[i].animationHandle = 0xFFFF;
  

  // Canvas
  RetroCtx->canvas        = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->canvasCount,   RetroP_Canvas);
  

  // Input
  RetroCtx->inputActions  = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxInputActions, RetroP_InputActionBinding);

  for(int i=0;i < settings->maxInputActions;i++)
  {
    RetroCtx->inputActions[i].action = 0xDEADBEEF;
  }


  // Sound
  RetroCtx->soundObject  = RetroP_Arena_ObtainImplArrayT(&RetroCtx->mem, settings->maxSoundObjects, RetroP_SoundObject);


  RetroCtx->framePresentation = FP_Normal;
  RetroCtx->frameAlpha = 0.78f;
  RetroCtx->frameBeta = 0.78f;

  Retro_Palette_MakeImpl(&RetroCtx->palette);

  if (settings->defaultPalette == 'DB16' || settings->defaultPalette == 'db16')
  {
    Retro_Palette_AddRGB(0x14, 0x0c, 0x1c ); // black
    Retro_Palette_AddRGB(0x44, 0x24, 0x34 ); // darkRed
    Retro_Palette_AddRGB(0x30, 0x34, 0x6d ); // darkBlue
    Retro_Palette_AddRGB(0x4e, 0x4a, 0x4e ); // darkGray
    Retro_Palette_AddRGB(0x85, 0x4c, 0x30 ); // brown
    Retro_Palette_AddRGB(0x34, 0x65, 0x24 ); // darkGreen
    Retro_Palette_AddRGB(0xd0, 0x46, 0x48 ); // red
    Retro_Palette_AddRGB(0x75, 0x71, 0x61 ); // lightGray
    Retro_Palette_AddRGB(0x59, 0x7d, 0xce ); // lightBlue
    Retro_Palette_AddRGB(0xd2, 0x7d, 0x2c ); // orange
    Retro_Palette_AddRGB(0x85, 0x95, 0xa1 ); // blueGray
    Retro_Palette_AddRGB(0x6d, 0xaa, 0x2c ); // lightGreen
    Retro_Palette_AddRGB(0xd2, 0xaa, 0x99 ); // peach
    Retro_Palette_AddRGB(0x6d, 0xc2, 0xca ); // cyan
    Retro_Palette_AddRGB(0xda, 0xd4, 0x5e ); // yellow
    Retro_Palette_AddRGB(0xde, 0xee, 0xd6 ); // white
    Retro_Palette_AddRGB(0xFF, 0x00, 0xFF ); // magenta/transparent

    RetroCtx->palette.fallback = 15;
    RetroCtx->palette.transparent = 16;
  }

  RetroCtx->mainTexture = SDL_CreateTexture(
    RetroPRenderer,
    SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_TARGET,
    settings->canvasWidth, 
    settings->canvasHeight);


  // Canvas and Canvas texture setup
  for (U8 i=0;i < settings->canvasCount;i++)
  {
    RetroCtx->canvas[i].texture = SDL_CreateTexture(
      RetroPRenderer,
      SDL_PIXELFORMAT_ABGR8888,
      SDL_TEXTUREACCESS_TARGET,
      settings->canvasWidth, 
      settings->canvasHeight);

    int flags = CNF_Clear | CNF_Render;
    if (i > 0)
      flags |= CNF_Blend;

    Retro_Canvas_Flags(i, flags, 0);
  }

  Retro_Canvas_Use(0);

  RetroCtx->library.initFunction();

  RetroCtx->quit = false;

  SDL_PauseAudio(0);
  RetroP_Restart();

  RetroCtx->frameCount = 0;
  Retro_Timer_Start(&RetroCtx->fpsTimer);
  Retro_Timer_Start(&RetroCtx->deltaTimer);

  RetroCtx->waitTime = (U32) (floorf(1000.0f / (float) (RETRO_DEFAULT_FRAME_RATE)));
  
  if (RetroCtx->waitTime > 1 && RetroPMinWaitTime > RetroCtx->waitTime)
  {
    RetroPMinWaitTime = RetroCtx->waitTime - 1;
  }

  return RetroCtx;
}

static void RetroP_ReleaseContext(RetroP_Context* context)
{
  SDL_CloseAudio();
  free(RetroCtx->arena.begin);
  free(RetroCtx);
  RetroCtx = NULL;
}

RETRO_API S32 Retro_Context_Id()
{
  if (RetroCtx == NULL)
    return -1;

  return RetroCtx->id;
}

RETRO_API U32 Retro_Context_Count()
{
  U32 count = 0;

  for (U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    if (RetroContexts[i] != NULL)
      count++;
  }

  return count;
}

// All contet ids. outNames must be an array of at least Retro_Context_Count()
RETRO_API void  Retro_Context_Ids(int* outNames)
{
  U32 count = 0;

  for (U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    if (RetroContexts[i] != NULL)
    {
      outNames[count] = RetroContexts[i]->id;
    }
  }
}

// Enable a context
RETRO_API void  Retro_Context_Enable(int name)
{
  for (U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    if (RetroContexts[i]->id == name)
    {
      RetroContexts[i]->enabled = true;
    }
  }
}

// Disable a context
RETRO_API void  Retro_Context_Disable(int name)

{
  for (U32 i=0;i < RETRO_MAX_CONTEXT;i++)
  {
    if (RetroContexts[i]->id == name)
    {
      RetroContexts[i]->enabled = false;
    }
  }
}

// Unload a context from memory (if a library also unload that library)
RETRO_API void  Retro_Context_Unload(int name)
{
  // TODO:

}

RetroP_Library RetroP_LoadLibrary(const char* file, bool asCopy)
{
  RetroP_Library library;
  memset(&library, 0, sizeof(RetroP_Library));

#if defined(RETRO_WINDOWS)
  // TODO: As Copy version
  //       Copies library to a temp name, and load that.
  
  library.winHandle = LoadLibraryA(file);
  assert(library.winHandle);
  
  library.initFunction      = (void(*)()) GetProcAddress(library.winHandle, "Init");
  assert(library.initFunction);
  library.startFunction     = (void(*)()) GetProcAddress(library.winHandle, "Start");
  assert(library.startFunction);
  library.stepFunction      = (void(*)()) GetProcAddress(library.winHandle, "Step");
  assert(library.stepFunction);
  library.settingsFunction  = (void(*)(Retro_Settings*)) GetProcAddress(library.winHandle, "Settings");
#else

#endif
  return library;
}

int Retro_Context_LoadFromInternal(Retro_Settings* settings, void(*initFunction)(), void(*startFunction)(), void(*stepFunction)(), U8 contextFlags)
{
  printf("[Retro] Creating Retro Context from runtime\n");

  RetroP_Context* prevCtx = RetroCtx;
  assert(settings);

  RetroP_InitialiseRetro(settings);
  RetroP_Library library;
#if defined(RETRO_WINDOWS)
  library.winHandle = 0;
#endif
  library.initFunction = initFunction;
  library.startFunction = startFunction;
  library.stepFunction = stepFunction;
  library.settingsFunction = NULL;

  RetroP_Context* newCtx = RetroP_InitContext(settings, &library);
  
  if (prevCtx == NULL)
  {
    printf("[Retro] Starting Main Loop\n");
    RetroP_RetroLoop();
  }

  RetroCtx = prevCtx;

  return newCtx->id; // @TODO Return Context ID.
}

int Retro_Context_LoadFromLibrary(const char* path, U8 flags)
{
  printf("[Retro] Creating Retro Context from library %s\n", path);
  
  RetroP_Context* prevCtx = RetroCtx;

  RetroP_Library library = RetroP_LoadLibrary(path, flags & RCF_LoadLibraryAsCopy);
  Retro_Settings settings = Retro_Default_Settings;

  if (library.settingsFunction)
  {
    library.settingsFunction(&settings);
  }

  RetroP_InitialiseRetro(&settings);
  RetroP_Context* newCtx = RetroP_InitContext(&settings, &library);

  if (prevCtx == NULL)
  {
    printf("[Retro] Starting Main Loop\n");
    RetroP_RetroLoop();
  }

  RetroCtx = prevCtx;
  return newCtx->id;  // @TODO Return ContextID
}

#undef RETRO_SDL_DRAW_PUSH_RGB
#undef RETRO_SDL_DRAW_POP_RGB
#undef RETRO_SDL_TO_RECT

#endif
