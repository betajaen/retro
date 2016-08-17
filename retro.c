#ifndef RETRO_C
#define RETRO_C

#include "retro.h"

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

typedef struct
{
  U16     windowWidth;
  U16     windowHeight;
  U16     canvasWidth;
  U16     canvasHeight;
  F32     soundVolume;
  Palette palette;
} RetroP_Settings;

typedef struct
{
  U8 *begin, *end, *current;
} RetroP_LinearAllocator;

void RetroP_LinearAllocator_Make(RetroP_LinearAllocator* allocator, U32 size)
{
  allocator->begin = malloc(size);
  allocator->end = allocator->begin + size;
  allocator->current = allocator->begin;
}

typedef struct
{
  int                name;
  U32                p;
} RetroP_ScopeStack;

typedef struct
{
  U32 action;
  int keys[RETRO_MAX_INPUT_BINDINGS];
  int axis[RETRO_MAX_INPUT_BINDINGS];
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
  BitmapHandle  bitmapHandle;
} RetroP_Bitmap;

typedef struct
{
  U32 length;
  U8* buffer;
  SDL_AudioSpec spec;
  SoundHandle soundHandle;
} RetroP_Sound;

typedef struct
{
  RetroP_Sound* sound;
  U32    p;
  U8     volume;
} Retro_SoundObject;

typedef struct
{
  BitmapHandle  bitmap;
  SDL_Rect      rect;
  SpriteHandle  spriteHandle;
} RetroP_Sprite;

typedef struct
{
  BitmapHandle    bitmap;
  U8              frameCount;
  U8              w, h;
  U16             frameLength;
  AnimationHandle animationHandle;
  Rect            frames[RETRO_MAX_ANIMATED_SPRITE_FRAMES];
} RetroP_Animation;

typedef union
{
  U32  q;
  U16  w[2];
  U8   b[4];
} RetroFourByteUnion;

#ifdef  RETRO_BROWSER

#define RETRO_BROWSER_PATH ((const char*) (RCTX->tempBrowserPath))

#define RETRO_MAKE_BROWSER_PATH(N) \
  RCTX->tempBrowserPath[0] = 0; \
  strcat(RCTX->tempBrowserPath, "assets/"); \
  strcat(RCTX->tempBrowserPath, name)

#endif


typedef struct
{
  SDL_Window*                  window;
  SDL_Renderer*                renderer;
  RetroP_ScopeStack            scopeStack[256];
  U32                          scopeStackIndex;
  RetroP_Settings              settings;


  // Canvas
  SDL_Texture*                 canvasTexture;
  SDL_Texture*                 canvasTextures[RETRO_CANVAS_COUNT];
  U8                           canvasFlags[RETRO_CANVAS_COUNT];
  U8                           canvasBackgroundColour[RETRO_CANVAS_COUNT];
  Retro_CanvasPresentation     framePresentation;
  float                        frameAlpha, frameBeta;

  // Input
  char                         inputChar;
  RetroP_InputCharState        inputCharState;
  RetroP_InputActionBinding    inputActions[RETRO_MAX_INPUT_ACTIONS];
  
  // Events and Timing
  bool                         quit;
  Timer                        fpsTimer, capTimer, deltaTimer;
  U32                          frameCount;
  U32                          deltaTime;
  float                        fps;

  // Audio
  RetroP_SoundDevice           soundDevice;
  Retro_SoundObject            soundObject[RETRO_MAX_SOUND_OBJECTS];
  micromod_sdl_context*        musicContext;
#ifdef RETRO_BROWSER
#endif

  // Sprites and Animation
  RetroP_Animation             animations[RETRO_MAX_ANIMATIONS];
  RetroP_Sprite                sprites[RETRO_MAX_SPRITES];
  RetroP_Bitmap                bitmaps[RETRO_MAX_BITMAPS];
  RetroP_Sound                 sounds[RETRO_MAX_SOUNDS];

  // Platform specific
#ifdef RETRO_BROWSER
  char                         tempBrowserPath[256];
  U8*                          musicFileData;
#endif

} RetroP_Context;

RetroP_Context*              RCTX;
RetroP_LinearAllocator       RETROP_ARENA;
RetroP_Animation*            RETROP_ANIMATIONS;
RetroP_Sprite*               RETROP_SPRITES;

#define RETRO_SDL_DRAW_PUSH_RGB(T, RGB) \
  SDL_Color T; U8 TAlpha;\
  SDL_GetRenderDrawColor(RCTX->renderer, &T.r, &T.g, &T.b, &TAlpha); \
  SDL_SetRenderDrawColor(RCTX->renderer, RGB.r, RGB.g, RGB.b, 0xFF);

#define RETRO_SDL_DRAW_POP_RGB(T) \
  SDL_SetRenderDrawColor(RCTX->renderer, T.r, T.g, T.b, 0xFF);

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

  HRSRC handle = FindResource(0, name, "RESOURCE");
  assert(handle);

  HGLOBAL data = LoadResource(0, handle);
  assert(data);

  void* ptr = LockResource(data);
  assert(ptr);

  DWORD dataSize = SizeofResource(0, handle);
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
    Colour colour;
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
  for (U32 i=0;i < RETRO_MAX_ANIMATIONS;i++)
  {
    if (RCTX->bitmaps[i].bitmapHandle == 0xFFFF)
    {
      RCTX->bitmaps[i].bitmapHandle = i;
      return &RCTX->bitmaps[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Bitmap* RetroP_Bitmap_Get(AnimationHandle handle)
{
  return &RCTX->bitmaps[handle];
} 

BitmapHandle Retro_Resources_LoadBitmap(const char* name,  U8 transparentIndex)
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

  SDL_Texture* texture = SDL_CreateTexture(RCTX->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  void* pixelsVoid;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;

  Palette* palette = &RCTX->settings.palette;
  
  for(U32 i=0, j=0;i < (width * height * 3);i+=3, j+=4)
  {
    Colour col;
    col.r = imageData[i+0];
    col.g = imageData[i+1];
    col.b = imageData[i+2];

    int bestIndex = 0x100;
    int bestDistance = 10000000;

    // Match nearest colour by using a treating the two colours as vectors, and matching against the closest distance between the two.
    for (U32 k=0;k < palette->count;k++)
    {
      Colour pal = palette->colours[k];

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
    
    Colour bestColour = palette->colours[bestIndex];

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
  for (U32 i=0;i < RETRO_MAX_SPRITES;i++)
  {
    if (RETROP_SPRITES[i].spriteHandle == 0xFFFF)
    {
      RETROP_SPRITES[i].spriteHandle = i;
      return &RETROP_SPRITES[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Sprite* RetroP_SpriteHandle_Get(SpriteHandle handle)
{
  return &RETROP_SPRITES[handle];
} 

SpriteHandle Retro_Sprites_LoadSprite(BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h)
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
  for (U32 i=0;i < RETRO_MAX_ANIMATIONS;i++)
  {
    if (RETROP_ANIMATIONS[i].animationHandle == 0xFFFF)
    {
      RETROP_ANIMATIONS[i].animationHandle = i;
      return &RETROP_ANIMATIONS[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Animation* Retro_AnimationHandle_Get(AnimationHandle handle)
{
  return &RETROP_ANIMATIONS[handle];
} 

AnimationHandle Retro_Animation_Load(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight, S32 frameOffsetX, S32 frameOffsetY)
{
  assert(numFrames < RETRO_MAX_ANIMATED_SPRITE_FRAMES);

  RetroP_Animation* animation = RetroP_AnimationHandle_GetFree();

  animation->bitmap = bitmap;
  animation->frameCount = numFrames;
  animation->frameLength = frameLengthMilliseconds;
  animation->w = frameWidth;
  animation->h = frameHeight;

  Rect frame;
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

AnimationHandle  Retro_Sprites_LoadAnimationH(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  return Retro_Animation_Load(bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, frameWidth, 0);
}

AnimationHandle  Retro_Sprites_LoadAnimationV(BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  return Retro_Animation_Load(bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, 0, frameHeight);
}

void Retro_Canvas_Use(U8 id)
{
  assert(id < RETRO_CANVAS_COUNT);
  RCTX->canvasTexture = RCTX->canvasTextures[id];
  SDL_SetRenderTarget(RCTX->renderer, RCTX->canvasTexture);
}

void Retro_Canvas_Clear()
{
  SDL_RenderClear(RCTX->renderer);
}

void Retro_Canvas_Flags(U8 id, U8 flags, U8 colour)
{
  assert(id < RETRO_CANVAS_COUNT);

  RCTX->canvasFlags[id] = flags;
  RCTX->canvasBackgroundColour[id] = colour;

  if (flags & CNF_Blend)
    SDL_SetTextureBlendMode(RCTX->canvasTextures[id], SDL_BLENDMODE_BLEND);
  else
    SDL_SetTextureBlendMode(RCTX->canvasTextures[id], SDL_BLENDMODE_NONE);

}

void  Retro_Canvas_Copy(BitmapHandle bitmap, Rect* dstRectangle, Rect* srcRectangle, U8 copyFlags)
{
  SDL_Rect d, s;

  if (dstRectangle == NULL)
  {
    d.x = 0;
    d.y = 0;
    d.w = RCTX->settings.canvasWidth;
    d.h = RCTX->settings.canvasHeight;
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
    s.w = RCTX->settings.canvasWidth;
    s.h = RCTX->settings.canvasHeight;
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
    SDL_RenderCopy(RCTX->renderer, texture, &s, &d);
  else
    SDL_RenderCopyEx(RCTX->renderer, texture, &s, &d, 0.0f, NULL, (SDL_RendererFlip) copyFlags);
}

void  Retro_Canvas_Copy2(BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags)
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
    SDL_RenderCopy(RCTX->renderer, texture, &s, &d);
  else
    SDL_RenderCopyEx(RCTX->renderer, texture, &s, &d, 0.0f, NULL, (SDL_RendererFlip) copyFlags);
}

void Retro_Canvas_Sprite(SpriteObject* spriteObject)
{
  assert(spriteObject);
  RetroP_Sprite* sprite = RetroP_SpriteHandle_Get(spriteObject->spriteHandle);
  assert(sprite);

  Rect d;
  d.x = spriteObject->x;
  d.y = spriteObject->y;
  d.w = sprite->rect.w;
  d.h = sprite->rect.h;

  Rect s;
  s.x = sprite->rect.x;
  s.y = sprite->rect.y;
  s.w = sprite->rect.w;
  s.h = sprite->rect.h;

  Retro_Canvas_Copy(sprite->bitmap, &d, &s, spriteObject->flags);
}

void  Retro_Canvas_Sprite2(SpriteHandle spriteHandle, S32 x, S32 y, U8 copyFlags)
{
  RetroP_Sprite* sprite = RetroP_SpriteHandle_Get(spriteHandle);
  assert(sprite);

  Rect d;
  d.x = x;
  d.y = y;
  d.w = sprite->rect.w;
  d.h = sprite->rect.h;

  Rect s;
  s.x = sprite->rect.x;
  s.y = sprite->rect.y;
  s.w = sprite->rect.w;
  s.h = sprite->rect.h;

  Retro_Canvas_Copy(sprite->bitmap, &d, &s, copyFlags);
}

void  Retro_Canvas_Animate(AnimationObject* animationObject, bool updateTiming)
{
  RetroP_Animation* animation = Retro_AnimationHandle_Get(animationObject->animationHandle);
  assert(animation);

  if (updateTiming && (animationObject->flags & SOF_Animation) != 0)
  {
    animationObject->frameTime += RCTX->deltaTime;

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

  Retro_Canvas_Animate2(animationObject->animationHandle, animationObject->x, animationObject->y, animationObject->frameNumber, animationObject->flags);
}

void  Retro_Canvas_Animate2(AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags)
{
  RetroP_Animation* animation = Retro_AnimationHandle_Get(animationHandle);
  assert(animation);
  assert(frame < animation->frameCount);

  Rect src = animation->frames[frame];
  Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = src.w;
  dst.h = src.h;

  Retro_Canvas_Copy(animation->bitmap, &dst, &src, copyFlags);
}

void Retro_Canvas_Flip()
{
  SDL_RenderPresent(RCTX->renderer);
}

void  Retro_Palette_MakeImpl(Palette* palette)
{
  assert(palette);
  palette->count = 0;
  palette->fallback = 1;
  palette->transparent = 0;
}

void  Retro_Palette_AddImpl(Palette* palette, Colour colour)
{
  assert(palette);
  assert(palette->count <= 255);
  palette->colours[palette->count] = colour;
  ++palette->count;
}

void  Retro_Palette_Add(Colour colour)
{
  Retro_Palette_AddImpl(&RCTX->settings.palette, colour);
}

void  Retro_Palette_Add2(U8 r, U8 g, U8 b)
{
  Colour colour;
  colour.r = r;
  colour.g = g;
  colour.b = b;
  Retro_Palette_AddImpl(&RCTX->settings.palette, colour);
}

void Retro_Palette_Add3(U32 rgb)
{
  RetroFourByteUnion c;
  c.q = rgb;

  Retro_Palette_AddImpl(&RCTX->settings.palette, Colour_Make(c.b[2], c.b[1], c.b[0]));
}

U8 Retro_Palette_IndexImpl(Palette* palette, Colour colour)
{
  assert(palette);

  for(int i=0;i < palette->count;++i)
  {
    Colour t = palette->colours[i];
    if (t.r == colour.r && t.g == colour.g && t.b == colour.b)
      return i;
  }
  return palette->fallback;
}

U8 Retro_Palette_Index(Colour colour)
{
  return Retro_Palette_IndexImpl(&RCTX->settings.palette, colour);
}

bool Retro_Palette_HasImpl(Palette* palette, Colour colour)
{
  assert(palette);

  for(int i=0;i < palette->count;++i)
  {
    Colour t = palette->colours[i];
    if (t.r == colour.r && t.g == colour.g && t.b == colour.b)
      return true;
  }
  return false;
}

bool Retro_Palette_Has(Colour colour)
{
  return Retro_Palette_HasImpl(&RCTX->settings.palette, colour);
}

Colour Retro_Palette_Get(U8 index)
{
  Palette* palette = &RCTX->settings.palette;
  return ((palette)->colours[index >= (palette)->count ? (palette)->fallback : index]);
}

void Retro_Palette_Set(U8 index, Colour colour)
{
  Palette* palette = &RCTX->settings.palette;
  palette->colours[index] = colour;
  palette->count = Retro_Max(palette->count, (index + 1));
}

void Retro_Palette_Set2(U8 index, U8 r, U8 g, U8 b)
{
  Palette* palette = &RCTX->settings.palette;
  palette->colours[index] = Colour_Make(r, g, b);
  palette->count = Retro_Max(palette->count, (index + 1));
}

void Retro_Palette_Copy(const Palette* src, Palette* dst)
{
  assert(src);
  assert(dst);

  dst->count = src->count;
  dst->fallback = src->fallback;
  dst->transparent = src->transparent;
  memcpy(dst->colours, src->colours, sizeof(src->colours));
}

U8     Hex_Char(char c)
{
  c = tolower(c);
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 0;
}

U8     Hex_U8(const char* str)
{
  char a = str[0], b = str[1];
  
  if (b == '\0')
    return 0;
  else if (a == '\0')
    return Hex_Char(a);
  else
    return (Hex_Char(a) << 4) | Hex_Char(b);
}

S8     Hex_S8(const char* str)
{
  char a = str[0], b = str[1];

  if (a == '\0')
    return 0;
  else if (b == '\0')
    return Hex_Char(a);
  else
    return Hex_Char(a) + Hex_Char(b) * 16;
}

U16    Hex_U16(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Hex_Char(*str);
  }
  return v;
}

S16    Hex_S16(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Hex_Char(*str);
  }
  return v;
}

U32    Hex_U32(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Hex_Char(*str);
  }
  return v;
}

S32    Hex_S32(const char* str)
{
  U16 v = 0;
  while(*str != '\0')
  {
    v = (v << 4) | Hex_Char(*str);
  }
  return v;
}

Colour Colour_Make(U8 r, U8 g, U8 b)
{
  Colour c;
  c.r = r;
  c.g = g;
  c.b = b;
  return c;
}

Colour Hex_Colour(const char* str)
{
  Colour c;
  c.r = 0;
  c.g = 0;
  c.b = 0;

  if (str == NULL)  // 0 => 0, 0, 0
    return c;

  int n = strlen(str);

  if (n == 1)       // R => Rr, Gg, Bb
  {
    U8 x = Hex_Char(str[0]) * 16;
    c.r = x;
    c.g = x;
    c.b = x;
  }
  else if (n == 2)  // Rr = Rr, Rr, Rr
  {
    U8 x = Hex_U8(str);
    c.r = x;
    c.g = x;
    c.b = x;
  }
  else if (n == 3)  // RGB = Rr, Gg, Bb
  {
    c.r = Hex_Char(str[0]);
    c.g = Hex_Char(str[1]);
    c.b = Hex_Char(str[2]);
  }
  else if (n == 4)  // RrGg =  0, 0, 0
  {
  }
  else if (n == 5)  // RrGgB = 0, 0, 0
  {
  }
  else if (n == 6)  // RrGgBb = Rr, Gg, Bb
  {
    c.r = Hex_U8(str);
    c.g = Hex_U8(str + 2);
    c.b = Hex_U8(str + 4);
  }
  else              // ...RrGgBb = Rr, Gg, Bb
  {
    c.r = Hex_U8(str + n - 6);
    c.g = Hex_U8(str + n - 4);
    c.b = Hex_U8(str + n - 2);
  }

  return c;
}

Point Point_Make(S32 x, S32 y)
{
  Point p;
  p.x = x;
  p.y = y;
  return p;
}

Colour Retro_Colour_ReadRGB(U8* p)
{
  Colour c;
  c.r = *(p + 0);
  c.g = *(p + 1);
  c.b = *(p + 2);
  return c;
}

Size Size_Make(U32 w, U32 h)
{
  Size s;
  s.w = w;
  s.h = h;
  return s;
}

Rect Rect_Make(S32 x, S32 y, S32 w, S32 h)
{
  Rect r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

U8* Arena_Obtain(U32 size)
{
  assert(RETROP_ARENA.current + size < RETROP_ARENA.end); // Ensure can fit.
  U8* mem = RETROP_ARENA.current;
  RETROP_ARENA.current += size;
  return mem;
}

void Arena_RewindPtr(U8* mem)
{
  assert(mem >= RETROP_ARENA.begin);
  assert(mem <= RETROP_ARENA.current);
  RETROP_ARENA.current = mem;
}

void Arena_RewindU32(U32 offset)
{
  assert(offset < RETRO_ARENA_SIZE);
  RETROP_ARENA.current = RETROP_ARENA.begin + offset;
}

U32 Arena_Current()
{
  return RETROP_ARENA.current - RETROP_ARENA.begin;
}

int  Arena_PctSize()
{
  U32 used = (RETROP_ARENA.current - RETROP_ARENA.begin);
  float pct = ((float) used / (float) RETRO_ARENA_SIZE);
  return (int) (pct * 100.0f);
}

typedef struct
{
  U8   header[4];
  U32  size;
  U32  scopeStackIndex;
  U32  musicSamples;
  char musicName[32];
} Retro_ArenaSave;

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
  U32 memSize = sizeof(Retro_ArenaSave);
  memSize += (RCTX->scopeStackIndex + 1) * sizeof(RetroP_ScopeStack);
  memSize += Arena_Current();

  *outSize = memSize;

  printf("Size = %i\n", memSize);

  U8* mem = (U8*) malloc(memSize);
  U8* p = mem;

  Retro_ArenaSave s;
  s.header[0] = 'R';
  s.header[1] = 'E';
  s.header[2] = 'T';
  s.header[3] = 'R';

  s.scopeStackIndex = RCTX->scopeStackIndex;
  s.size = (RETROP_ARENA.current - RETROP_ARENA.begin);
  
  if (RCTX->musicContext != NULL)
    s.musicSamples = RCTX->musicContext->samples_remaining;
  else
    s.musicSamples = 0;

  p = Retro_SaveToMem(p, &s, sizeof(Retro_ArenaSave));

  for(U32 i=0;i < (s.scopeStackIndex + 1);i++)
  {
    //fwrite(&RCTX->scopeStack[i], sizeof(ScopeStack), 1,);
    p = Retro_SaveToMem(p, &RCTX->scopeStack[i], sizeof(RetroP_ScopeStack));
  }

  p = Retro_SaveToMem(p, RETROP_ARENA.begin, s.size);
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

  Retro_ArenaSave l;

  p = Retro_ReadFromMem(p, &l, sizeof(Retro_ArenaSave));

  assert(l.header[0] == 'R');
  assert(l.header[1] == 'E');
  assert(l.header[2] == 'T');
  assert(l.header[3] == 'R');
  assert(l.size < RETRO_ARENA_SIZE);
 
  RCTX->scopeStackIndex = l.scopeStackIndex;
  
  for(U32 i=0;i < l.scopeStackIndex + 1;i++)
  {
    //fread(&RCTX->scopeStack[i], sizeof(ScopeStack), 1, f);
    p = Retro_ReadFromMem(p, &RCTX->scopeStack[i], sizeof(RetroP_ScopeStack));
  }

  p = Retro_ReadFromMem(p, RETROP_ARENA.begin, l.size);

  RETROP_ARENA.current = RETROP_ARENA.begin + l.size;


  if (loadMusic && RCTX->musicContext != NULL)
  {
    // RCTX->musicContext->samples_remaining = l.musicSamples;
    // micromod_set_position(l.musicSamples);
  }

}

void Retro_Scope_Push(int name)
{
  assert(RCTX->scopeStackIndex < 256);

  ++RCTX->scopeStackIndex;
  RetroP_ScopeStack* scope = &RCTX->scopeStack[RCTX->scopeStackIndex];

  scope->name = name;
  scope->p = Arena_Current();
}

int Retro_Scope_Name()
{
  RetroP_ScopeStack* scope = &RCTX->scopeStack[RCTX->scopeStackIndex];
  return scope->name;
}

U8* Retro_Scope_Obtain(U32 size)
{
  RetroP_ScopeStack* scope = &RCTX->scopeStack[RCTX->scopeStackIndex];
  assert(scope->p + size < RETRO_ARENA_SIZE); // Ensure can fit.
  return Arena_Obtain(size);
}

void Retro_Scope_Rewind()
{
  RetroP_ScopeStack* scope = &RCTX->scopeStack[RCTX->scopeStackIndex];
  Arena_RewindU32(scope->p);
}

void Retro_Scope_Pop()
{
  assert(RCTX->scopeStackIndex > 0);
  Retro_Scope_Rewind();
  --RCTX->scopeStackIndex;
}

void Retro_Canvas_DrawPalette(S32 Y)
{
  Retro_Canvas_DrawPalette2(Y, 0, RCTX->settings.palette.count - 1);
}

void Retro_Canvas_DrawPalette2(S32 Y, U8 from, U8 to)
{
  if (from > to)
  {
    U8 t = to;
    to = from;
    from = t;
  }

  Rect rect;
  rect.x = 0;
  rect.y = Y;
  rect.w = RCTX->settings.canvasHeight / 16;
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

void Retro_Canvas_DrawBox(U8 colour, Rect rect)
{
  Colour rgb = Retro_Palette_Get(colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  RETRO_SDL_DRAW_PUSH_RGB(t, rgb);

  SDL_RenderDrawRect(RCTX->renderer, &dst);

  RETRO_SDL_DRAW_POP_RGB(t);
}

void Retro_Canvas_DrawRectangle(U8 colour, Rect rect)
{
  Colour rgb = Retro_Palette_Get(colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  SDL_SetRenderDrawColor(RCTX->renderer, rgb.r, rgb.g, rgb.b, 0xFF);
  SDL_RenderFillRect(RCTX->renderer, &dst);
  SDL_SetRenderDrawColor(RCTX->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

char* gFmtScratch;

void  Retro_Canvas_MonoPrint(S32 x, S32 y, S32 w, Font* font, U8 colour, const char* str)
{
  assert(font);
  assert(str);

  Colour rgb = Retro_Palette_Get(colour);

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

    SDL_RenderCopy(RCTX->renderer, texture, &s, &d);

    d.x = ox + w;
  }

  RETRO_SDL_TEXTURE_POP_RGB(t, texture);
}

void Retro_Canvas_Print(S32 x, S32 y, Font* font, U8 colour, const char* str)
{
  assert(font);
  assert(str);

  Colour rgb = Retro_Palette_Get(colour);

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

    SDL_RenderCopy(RCTX->renderer, texture, &s, &d);

    d.x += d.w;
  }

  RETRO_SDL_TEXTURE_POP_RGB(t, texture);

}

void Retro_Canvas_Printf(S32 x, S32 y, Font* font, U8 colour, const char* fmt, ...)
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
  RCTX->framePresentation = presentation;
  RCTX->frameAlpha = alpha;
  RCTX->frameBeta = beta;
}

void Retro_Sprites_NewAnimationObject(AnimationObject* inAnimatedSpriteObject, AnimationHandle animation, S32 x, S32 y)
{
  assert(inAnimatedSpriteObject);

  inAnimatedSpriteObject->animationHandle = animation;
  inAnimatedSpriteObject->flags = 0;
  inAnimatedSpriteObject->frameNumber = 0;
  inAnimatedSpriteObject->frameTime = 0;
  inAnimatedSpriteObject->x = x;
  inAnimatedSpriteObject->y = y;
}

void Retro_Sprites_PlayAnimationObject(AnimationObject* inAnimatedSpriteObject, bool playing, bool loop)
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

void Retro_Sprites_SetAnimationObject(AnimationObject* animatedSpriteObject, AnimationHandle newAnimation, bool animate)
{
  assert(animatedSpriteObject);

  animatedSpriteObject->animationHandle = newAnimation;
  if (animate)
    animatedSpriteObject->flags |= SOF_Animation;
  else
    animatedSpriteObject->flags &= ~SOF_Animation;
  animatedSpriteObject->frameNumber = 0;
}

void  Retro_Debug(Font* font)
{
  assert(font);
  RetroFourByteUnion f;
  f.q = Retro_Scope_Name();
  
  U32 soundObjectCount = 0;
  for(U32 i=0;i < RETRO_MAX_SOUND_OBJECTS;i++)
  {
    Retro_SoundObject* soundObj = &RCTX->soundObject[i];

    if (soundObj->sound != NULL)
      soundObjectCount++;
  }

  int music = -1;

  if (RCTX->musicContext != NULL)
  {
    music = (int) 100 - (((float) RCTX->musicContext->samples_remaining / (float) RCTX->musicContext->length) *100.0f);
  }

  Retro_Canvas_Printf(0, RCTX->settings.canvasHeight - font->height, font, RCTX->settings.palette.fallback, "Scope=%c%c%c%c Mem=%i%% FPS=%.2g Dt=%i Snd=%i, Mus=%i", f.b[3], f.b[2], f.b[1], f.b[0], Arena_PctSize(), RCTX->fps, RCTX->deltaTime, soundObjectCount, music);
}

RetroP_Sound* RetroP_Sound_GetFree()
{
  for (U32 i=0;i < RETRO_MAX_SOUNDS;i++)
  {
    if (RCTX->sounds[i].soundHandle == 0xFFFF)
    {
      RCTX->sounds[i].soundHandle = i;
      return &RCTX->sounds[i];
    }
  }
  assert(true);
  return NULL;
}

RetroP_Sound* RetroP_Sound_Get(AnimationHandle handle)
{
  return &RCTX->sounds[handle];
} 

SoundHandle Retro_Resources_LoadSound(const char* name)
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

  if (sound->spec.format != RCTX->soundDevice.specification.format || sound->spec.freq != RCTX->soundDevice.specification.freq || sound->spec.channels != RCTX->soundDevice.specification.channels)
  {
    // Do a conversion
    SDL_AudioCVT cvt;
    SDL_BuildAudioCVT(&cvt, sound->spec.format, sound->spec.channels, sound->spec.freq, RCTX->soundDevice.specification.format, RCTX->soundDevice.specification.channels, RCTX->soundDevice.specification.freq);

    cvt.buf = malloc(sound->length * cvt.len_mult);
    memcpy(cvt.buf, sound->buffer, sound->length);
    cvt.len = sound->length;
    SDL_ConvertAudio(&cvt);
    SDL_FreeWAV(sound->buffer);

    sound->buffer = cvt.buf;
    sound->length = cvt.len_cvt;
    sound->spec = RCTX->soundDevice.specification;

    // printf("Loaded Audio %s but had to convert it into a internal format.\n", name);
  }
  else
  {
   // printf("Loaded Audio %s\n", name);
  }

  return sound->soundHandle;

}

void  Retro_Audio_PlaySound(SoundHandle soundHandle, U8 volume)
{
  RetroP_Sound* sound = RetroP_Sound_Get(soundHandle);

  for(U32 i=0;i < RETRO_MAX_SOUND_OBJECTS;i++)
  {
    Retro_SoundObject* soundObj = &RCTX->soundObject[i];
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
  for(U32 i=0;i < RETRO_MAX_SOUND_OBJECTS;i++)
  {
    Retro_SoundObject* soundObj = &RCTX->soundObject[i];

    soundObj->sound = NULL;
    soundObj->p = 0;
    soundObj->volume = 0;
  }
}

void Retro_Audio_PlayMusic(const char* name)
{
  if (RCTX->musicContext != NULL)
  {
    Retro_Audio_StopMusic();
  }

  RCTX->musicContext = malloc(sizeof(micromod_sdl_context));
  memset(RCTX->musicContext, 0, sizeof(micromod_sdl_context));

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

  RCTX->musicFileData = malloc(dataLength);
  fread(RCTX->musicFileData, dataLength, 1, f);
  fclose(f);

  data = RCTX->musicFileData;
#endif

  micromod_initialise(data, SAMPLING_FREQ * OVERSAMPLE);
  RCTX->musicContext->samples_remaining = micromod_calculate_song_duration();
  RCTX->musicContext->length = RCTX->musicContext->samples_remaining;

}

void Retro_Audio_StopMusic()
{
  if (RCTX->musicContext == NULL)
  {
    return;
  }

  #if defined(RETRO_BROWSER)
    free(RCTX->musicFileData);
    RCTX->musicFileData = NULL;
  #endif

  free(RCTX->musicContext);
  RCTX->musicContext = NULL;
}

void Retro_SDL_SoundCallback(void* userdata, U8* stream, int streamLength)
{
  SDL_memset(stream, 0, streamLength);

  if (RCTX->musicContext != NULL)
  {

    // int uSize = (RCTX->soundDevice.specification.format == AUDIO_S16 ? sizeof(short) : sizeof(float));

    long count = 0;

    if (RCTX->soundDevice.specification.format == AUDIO_S16)
      count = streamLength / 2;
    else
      count = streamLength / 4;
    
    if( RCTX->musicContext->samples_remaining < count ) {
      /* Clear output.*/
      count = RCTX->musicContext->samples_remaining;
    }

    if( count > 0 ) {
      /* Get audio from replay.*/

      memset( RCTX->musicContext->mix_buffer, 0, count * NUM_CHANNELS * sizeof( short ) );
      micromod_get_audio( RCTX->musicContext->mix_buffer, count );
      
      if (RCTX->soundDevice.specification.format == AUDIO_S16)
        micromod_sdl_downsample( RCTX->musicContext, RCTX->musicContext->mix_buffer, (short *) stream, count );
      else
        micromod_sdl_downsample_float( RCTX->musicContext, RCTX->musicContext->mix_buffer, (float*) stream, count);
      
      RCTX->musicContext->samples_remaining -= count;
    }
    else
    {
      RCTX->musicContext->samples_remaining = RCTX->musicContext->length;
    }
  }

  for(U32 i=0;i < RETRO_MAX_SOUND_OBJECTS;i++)
  {
    Retro_SoundObject* soundObj = &RCTX->soundObject[i];

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

void  Retro_Font_Make(Font* font)
{
  assert(font);
  memset(font->widths, 0, sizeof(font->widths));
  font->height = 0;
  font->bitmap = 0xFFFF;
}

void Retro_Resources_LoadFont(const char* name, Font* outFont, Colour markerColour, Colour transparentColour)
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

  SDL_Texture* texture = SDL_CreateTexture(RCTX->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height - 1);

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
    Colour col = Retro_Colour_ReadRGB(&imageData[i]);
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
    Colour col = Retro_Colour_ReadRGB(&imageData[j]);

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

void  Retro_Resources_LoadFontFixed(const char* name, Font* outFont, U8 w, Colour transparentColour)
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

  SDL_Texture* texture = SDL_CreateTexture(RCTX->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height - 1);

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
    Colour col = Retro_Colour_ReadRGB(&imageData[j]);

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

  switch(RCTX->inputCharState)
  {
    default:
    case ICS_None:
    return 0;
    case ICS_Character:
    {
      if (len < capacity)
      {
        str[len] = RCTX->inputChar;
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
  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    RetroP_InputActionBinding* binding = &RCTX->inputActions[i];
    if (binding->action == action)
      return binding;
  }
  return NULL;
}

RetroP_InputActionBinding* Retro_Input_MakeAction(int action)
{
  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    RetroP_InputActionBinding* binding = &RCTX->inputActions[i];
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

  for (U32 i=0;i < RETRO_MAX_INPUT_BINDINGS;i++)
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

  for (U32 i=0;i < RETRO_MAX_INPUT_BINDINGS;i++)
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

void  Retro_Timer_Make(Timer* timer)
{
  assert(timer);
  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Retro_Timer_Start(Timer* timer)
{
  assert(timer);
  timer->flags = TF_Started;
  timer->start = SDL_GetTicks();
  timer->paused = 0;
}

void  Retro_Timer_Stop(Timer* timer)
{
  assert(timer);

  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Retro_Timer_Pause(Timer* timer)
{
  assert(timer);

  if (timer->flags == TF_Started)
  {
    timer->flags |= TF_Paused;
    timer->paused = SDL_GetTicks() - timer->start;
    timer->start = 0;
  }
}

void  Retro_Timer_Unpause(Timer* timer)
{
  assert(timer);

  if (timer->flags == 3 /* Started | Paused */)
  {
    timer->flags = TF_Started; // &= ~Paused
    timer->start = SDL_GetTicks() - timer->paused;
    timer->paused = 0;
  }
}

U32   Retro_Timer_Ticks(Timer* timer)
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

bool  Retro_Timer_Started(Timer* timer)
{
  return timer->flags >= TF_Started;
}

bool  Retro_Timer_Paused(Timer* timer)
{
  return timer->flags >= TF_Paused;
}

void Canvas_Present()
{
  switch(RCTX->framePresentation)
  {
    case FP_Normal:
    {
      for (int i=0;i < RETRO_CANVAS_COUNT;i++)
      {
        if (RCTX->canvasFlags[i] & CNF_Render)
        {
          SDL_RenderCopy(RCTX->renderer, RCTX->canvasTextures[i], NULL, NULL);
        }
      }
    }
    break;
    case FP_WaveH:
    {
      U32 accuracy = 2;

      for (U32 u=0;u < RETRO_WINDOW_DEFAULT_HEIGHT;u+=accuracy)
      {
        SDL_Rect src;
        SDL_Rect dst;
        src.x = 0;
        src.y = u;
        src.w = RETRO_CANVAS_DEFAULT_WIDTH;
        src.h = accuracy;

        dst = src;

        float x0 = (float) u / (float) RETRO_WINDOW_DEFAULT_HEIGHT;
        dst.x = (sin((RCTX->frameCount % 1000) * RCTX->frameAlpha + x0 * 3.14f) * RCTX->frameBeta);
        dst.y = u * 2;
        dst.w = RETRO_WINDOW_DEFAULT_WIDTH;
        dst.h = accuracy * 2;

        for (int i=0;i < RETRO_CANVAS_COUNT;i++)
        {
          if (RCTX->canvasFlags[i] & CNF_Render)
          {
            SDL_RenderCopy(RCTX->renderer, RCTX->canvasTextures[i], &src, &dst);
          }
        }
      }
    }
    break;
    case FP_WaveV:
    {
      U32 accuracy = 2;

      for (U32 u=0;u < RETRO_WINDOW_DEFAULT_HEIGHT;u+=accuracy)
      {
        SDL_Rect src;
        SDL_Rect dst;
        src.x = u;
        src.y = 0;
        src.w = accuracy;
        src.h = RETRO_CANVAS_DEFAULT_HEIGHT;

        dst = src;

        float y0 = (float) u / (float) RETRO_WINDOW_DEFAULT_HEIGHT;
        dst.x = u * 2;
        dst.y = (sin((RCTX->frameCount % 1000) * RCTX->frameAlpha + y0 * 3.14f) * RCTX->frameBeta);
        dst.w = accuracy * 2;
        dst.h = RETRO_WINDOW_DEFAULT_HEIGHT;

        for (int i=0;i < RETRO_CANVAS_COUNT;i++)
        {
          if (RCTX->canvasFlags[i] & CNF_Render)
          {
            SDL_RenderCopy(RCTX->renderer, RCTX->canvasTextures[i], &src, &dst);
          }
        }
      }
    }
    break;
    case FP_Scale:
    {

      SDL_Rect src;
      SDL_Rect dst;
      src.w = (float) RETRO_CANVAS_DEFAULT_WIDTH * RCTX->frameAlpha;
      src.h = (float) RETRO_CANVAS_DEFAULT_HEIGHT * RCTX->frameBeta;
      src.x = 0; //src.w / 2;
      src.y = 0; //src.h / 2;

      dst = src;
      dst.x = RETRO_WINDOW_DEFAULT_WIDTH / 2 - (src.w);
      dst.y = RETRO_WINDOW_DEFAULT_HEIGHT / 2 - (src.h);
      dst.w *= 2;
      dst.h *= 2;

      for (int i=0;i < RETRO_CANVAS_COUNT;i++)
      {
        if (RCTX->canvasFlags[i] & CNF_Render)
        {
          SDL_RenderCopy(RCTX->renderer, RCTX->canvasTextures[i], &src, &dst);
        }
      }
    }
    break;
  }
}

#ifdef RETRO_IS_LIBRARY

void Lib_Init()
{
}

void Lib_Start()
{
}

void Lib_Step()
{
}

#endif

void Frame()
{

  Retro_Timer_Start(&RCTX->capTimer);

  RCTX->deltaTime = Retro_Timer_Ticks(&RCTX->deltaTimer);

  SDL_Event event;
  RCTX->inputCharState = ICS_None;

  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
      {
        RCTX->quit = true;
      }
      break;
      case SDL_TEXTINPUT:
      {
        RCTX->inputChar = event.text.text[0];
        RCTX->inputCharState = ICS_Character;
      }
      break;
      case SDL_KEYDOWN:
      {

        if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
          RCTX->inputCharState = ICS_Backspace;
        }
        else if (event.key.keysym.sym == SDLK_RETURN)
        {
          RCTX->inputCharState = ICS_Enter;
        }
      }
      break;
    }
  }

  RCTX->fps = RCTX->frameCount / (Retro_Timer_Ticks(&RCTX->fpsTimer) / 1000.0f);
  if (RCTX->fps > 200000.0f)
  {
    RCTX->fps = 0.0f;
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  for (U32 i=0;i < RETRO_MAX_INPUT_ACTIONS;i++)
  {
    RetroP_InputActionBinding* binding = &RCTX->inputActions[i];
    if (binding->action == 0xDEADBEEF)
      break;

    binding->lastState = binding->state;
    binding->state = 0;

    for (U32 j=0; j < RETRO_MAX_INPUT_BINDINGS;j++)
    {
      int key = binding->keys[j];

      if (key == SDL_SCANCODE_UNKNOWN || key >= SDL_NUM_SCANCODES)
        break;

      binding->state |= (state[key] != 0) ? 1 : 0;
    }

    // @TODO Axis
  }
  
  for (U8 i=0;i < RETRO_CANVAS_COUNT;i++)
  {
    if (RCTX->canvasFlags[i] & CNF_Clear)
    {
      Retro_Canvas_Use(i);
      Colour col = Retro_Palette_Get(RCTX->canvasBackgroundColour[i]);
      SDL_SetRenderDrawColor(RCTX->renderer, col.r, col.g, col.b, 0x00);
      Retro_Canvas_Clear();
      SDL_SetRenderDrawColor(RCTX->renderer, 0xFF, 0xFF, 0xFF, 0x00);
    }
  }

  Retro_Canvas_Use(0);
  
  #ifdef RETRO_IS_LIBRARY
    Lib_Step();
  #else
    Step();
  #endif

  SDL_SetRenderTarget(RCTX->renderer, NULL);

  Canvas_Present();

  Retro_Canvas_Flip();
  
  ++RCTX->frameCount;
  
  Retro_Timer_Start(&RCTX->deltaTimer);
}

void Restart()
{
  RETROP_ARENA.current = RETROP_ARENA.begin;

  RCTX->scopeStackIndex = 0;
  RCTX->scopeStack[0].p = 0;
  RCTX->scopeStack[0].name = 'INIT';

#ifdef RETRO_IS_LIBRARY
  Lib_Start();
#else
  Start();
#endif
}

#ifdef RETRO_WINDOWS
int main(int argc, char *argv[])
#endif
#ifdef RETRO_BROWSER
int main(int argc, char **argv)
#endif
{

  SDL_Init(SDL_INIT_EVERYTHING);

  RCTX = (RetroP_Context*) malloc(sizeof(RetroP_Context));
  memset(RCTX, 0, sizeof(RetroP_Context));


  RETROP_ARENA.begin = malloc(RETRO_ARENA_SIZE);
  RETROP_ARENA.current = RETROP_ARENA.begin;
  RETROP_ARENA.end = RETROP_ARENA.begin + RETRO_ARENA_SIZE;

  RETROP_ANIMATIONS = &RCTX->animations[0];
  RETROP_SPRITES    = &RCTX->sprites[0];

  memset(RETROP_ARENA.begin, 0, RETRO_ARENA_SIZE);

  gFmtScratch = malloc(1024);

  memset(RCTX->inputActions, 0, sizeof(RCTX->inputActions));

  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    RCTX->inputActions[i].action = 0xDEADBEEF;
  }

  RCTX->settings.windowWidth = RETRO_WINDOW_DEFAULT_WIDTH;
  RCTX->settings.windowHeight = RETRO_WINDOW_DEFAULT_HEIGHT;
  RCTX->settings.canvasWidth = RETRO_CANVAS_DEFAULT_WIDTH;
  RCTX->settings.canvasHeight = RETRO_CANVAS_DEFAULT_HEIGHT;
  
  memset(RETROP_ANIMATIONS, 0, RETRO_MAX_ANIMATIONS * sizeof(RetroP_Animation));
  memset(RETROP_SPRITES, 0, RETRO_MAX_SPRITES * sizeof(RetroP_Sprite));

  for(U32 i=0;i < RETRO_MAX_ANIMATIONS;i++)
    RETROP_ANIMATIONS[i].animationHandle = 0xFFFF;

  for(U32 i=0;i < RETRO_MAX_SPRITES;i++)
    RETROP_SPRITES[i].spriteHandle = 0xFFFF;

  for(U32 i=0;i < RETRO_MAX_BITMAPS;i++)
    RCTX->bitmaps[i].bitmapHandle = 0xFFFF;

  for(U32 i=0;i < RETRO_MAX_SOUNDS;i++)
    RCTX->sounds[i].soundHandle = 0xFFFF;

  RCTX->window = SDL_CreateWindow( 
    RETRO_WINDOW_CAPTION,
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED,
    RCTX->settings.windowWidth,
    RCTX->settings.windowHeight,
    SDL_WINDOW_SHOWN
  );

  memset(&RCTX->soundObject, 0, sizeof(RCTX->soundObject));
  memset(&RCTX->soundDevice, 0, sizeof(RetroP_SoundDevice));

  RCTX->musicContext = NULL;

  SDL_AudioSpec want, got;
  memset(&want, 0, sizeof(want));
  memset(&got, 0, sizeof(got));

  want.freq = RETRO_AUDIO_FREQUENCY;
  want.format = AUDIO_S16;
  want.channels = RETRO_AUDIO_CHANNELS;
  want.samples = RETRO_AUDIO_SAMPLES;
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

  RCTX->soundDevice.specification = got;
  RCTX->musicContext = NULL;

#ifdef RETRO_BROWSER
  RCTX->musicFileData = NULL;
#endif

  RCTX->renderer = SDL_CreateRenderer(RCTX->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
  RCTX->framePresentation = FP_Normal;
  RCTX->frameAlpha = 0.78f;
  RCTX->frameBeta = 0.78f;

  Retro_Palette_MakeImpl(&RCTX->settings.palette);

#if (RETRO_DEFAULT_PALETTE == 'DB16' || RETRO_DEFAULT_PALETTE == 'db16')
  Retro_Palette_Add2(0x14, 0x0c, 0x1c ); // black
  Retro_Palette_Add2(0x44, 0x24, 0x34 ); // darkRed
  Retro_Palette_Add2(0x30, 0x34, 0x6d ); // darkBlue
  Retro_Palette_Add2(0x4e, 0x4a, 0x4e ); // darkGray
  Retro_Palette_Add2(0x85, 0x4c, 0x30 ); // brown
  Retro_Palette_Add2(0x34, 0x65, 0x24 ); // darkGreen
  Retro_Palette_Add2(0xd0, 0x46, 0x48 ); // red
  Retro_Palette_Add2(0x75, 0x71, 0x61 ); // lightGray
  Retro_Palette_Add2(0x59, 0x7d, 0xce ); // lightBlue
  Retro_Palette_Add2(0xd2, 0x7d, 0x2c ); // orange
  Retro_Palette_Add2(0x85, 0x95, 0xa1 ); // blueGray
  Retro_Palette_Add2(0x6d, 0xaa, 0x2c ); // lightGreen
  Retro_Palette_Add2(0xd2, 0xaa, 0x99 ); // peach
  Retro_Palette_Add2(0x6d, 0xc2, 0xca ); // cyan
  Retro_Palette_Add2(0xda, 0xd4, 0x5e ); // yellow
  Retro_Palette_Add2(0xde, 0xee, 0xd6 ); // white
  Retro_Palette_Add2(0xFF, 0x00, 0xFF ); // magenta/transparent

  RCTX->settings.palette.fallback = 15;
  RCTX->settings.palette.transparent = 16;
#endif

  for (U8 i=0;i < RETRO_CANVAS_COUNT;i++)
  {
    RCTX->canvasTextures[i] = SDL_CreateTexture(RCTX->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, RETRO_CANVAS_DEFAULT_WIDTH, RETRO_CANVAS_DEFAULT_HEIGHT);
    int flags = CNF_Clear | CNF_Render;
    if (i > 0)
      flags |= CNF_Blend;

    Retro_Canvas_Flags(i, flags, 0);
  }

  Retro_Canvas_Use(0);

  #ifdef RETRO_IS_LIBRARY
    Lib_Init();
  #else
    Init();
  #endif

  RCTX->quit = false;

  SDL_PauseAudio(0);
  Restart();

  RCTX->frameCount = 0;
  Retro_Timer_Start(&RCTX->fpsTimer);
  Retro_Timer_Start(&RCTX->deltaTimer);

  #ifdef RETRO_WINDOWS

  while(RCTX->quit == false)
  {
    Frame();
    float frameTicks = Retro_Timer_Ticks(&RCTX->capTimer);
    if (frameTicks < (1000.0f / RETRO_FRAME_RATE))
    {
      SDL_Delay((1000.0f / RETRO_FRAME_RATE) - frameTicks);
    }
  }

  #endif

  #ifdef RETRO_BROWSER

  emscripten_set_main_loop(Frame, RETRO_FRAME_RATE, 1);

  #endif

  free(RETROP_ARENA.begin);
  free(RCTX);

  SDL_CloseAudio();
  SDL_Quit();
  return 0;
}

#undef RETRO_SDL_DRAW_PUSH_RGB
#undef RETRO_SDL_DRAW_POP_RGB
#undef RETRO_SDL_TO_RECT

#endif
