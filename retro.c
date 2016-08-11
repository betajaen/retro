#ifndef RETRO_C
#define RETRO_C

#include "retro.h"

#ifdef RETRO_WINDOWS
#   include "windows.h"
#endif

#define LODEPNG_NO_COMPILE_ENCODER
#include "ref/lodepng.c"
#include "SDL_main.h"
#include "assert.h"

#ifdef RETRO_BROWSER
#include <emscripten.h>
#endif

Colour kDefaultPalette[] = {
  { 0xFF, 0x00, 0xFF },
  { 0x14, 0x0c, 0x1c },
  { 0x44, 0x24, 0x34 },
  { 0x30, 0x34, 0x6d },
  { 0x4e, 0x4a, 0x4e },
  { 0x85, 0x4c, 0x30 },
  { 0x34, 0x65, 0x24 },
  { 0xd0, 0x46, 0x48 },
  { 0x75, 0x71, 0x61 },
  { 0x59, 0x7d, 0xce },
  { 0xd2, 0x7d, 0x2c },
  { 0x85, 0x95, 0xa1 },
  { 0x6d, 0xaa, 0x2c },
  { 0xd2, 0xaa, 0x99 },
  { 0x6d, 0xc2, 0xca },
  { 0xda, 0xd4, 0x5e },
  { 0xde, 0xee, 0xd6 }
};

void (*FinalizerFn)(void* ptr);

typedef struct
{
  U8 *begin, *end, *current;
} LinearAllocator;

void LinearAllocator_Make(LinearAllocator* allocator, U32 size)
{
  allocator->begin = malloc(size);
  allocator->end = allocator->begin + size;
  allocator->current = allocator->begin;
}
struct Finaliser;
struct Finaliser
{
  void (*fn)(void*);
  void* mem;
  struct Finaliser* head;
};

typedef struct
{
  int                name;
  U8*                offset;
  struct Finaliser*  head;
} ScopeStack;

typedef struct
{
  U32 action;
  int keys[RETRO_MAX_INPUT_BINDINGS];
  int axis[RETRO_MAX_INPUT_BINDINGS];
  S16 state, lastState;
} InputActionBinding;

typedef enum
{
  ICS_None,
  ICS_Character,
  ICS_Backspace,
  ICS_Enter
} InputCharState;

SDL_Window*         gWindow;
SDL_Renderer*       gRenderer;
SDL_Texture*        gCanvasTexture;
SDL_Texture*        gCanvasTextures[RETRO_CANVAS_COUNT];
U8                  gCanvasFlags[RETRO_CANVAS_COUNT];
U8                  gCanvasBackgroundColour[RETRO_CANVAS_COUNT];
Settings            gSettings;
Size                gCanvasSize;
LinearAllocator     gArena;
ScopeStack          gScopeStack[RETRO_ARENA_STACK_SIZE];
U32                 gScopeStackIndex;
char                gInputChar;
InputCharState      gInputCharState;
InputActionBinding  gInputActions[RETRO_MAX_INPUT_ACTIONS];
bool                gQuit;
Timer               gFpsTimer, gCapTimer, gDeltaTimer;
U32                 gCountedFrames;
U32                 gDeltaTime;
float               gFps;

typedef union
{
  U32  q;
  U16  w[2];
  U8   b[4];
} RetroFourByteUnion;

#define RETRO_SDL_DRAW_PUSH_RGB(T, RGB) \
  SDL_Color T; U8 TAlpha;\
  SDL_GetRenderDrawColor(gRenderer, &T.r, &T.g, &T.b, &TAlpha); \
  SDL_SetRenderDrawColor(gRenderer, RGB.r, RGB.g, RGB.b, 0xFF);

#define RETRO_SDL_DRAW_POP_RGB(T) \
  SDL_SetRenderDrawColor(gRenderer, T.r, T.g, T.b, 0xFF);

#define RETRO_SDL_TEXTURE_PUSH_RGB(T, TEXTURE, RGB) \
  SDL_Color T; \
  SDL_GetTextureColorMod(TEXTURE, &T.r, &T.g, &T.b); \
  SDL_SetTextureColorMod(TEXTURE, RGB.r, RGB.g, RGB.b);

#define RETRO_SDL_TEXTURE_POP_RGB(T, TEXTURE) \
  SDL_SetTextureColorMod(TEXTURE, T.r, T.g, T.b);

#define RETRO_SDL_TO_RECT(SRC, DST) \
  DST.x = SRC.left;\
  DST.y = SRC.top;\
  DST.w = SRC.right - SRC.left;\
  DST.h = SRC.bottom - SRC.top;

#ifdef RETRO_BROWSER

char gTempBrowserPath[256];

#define RETRO_BROWSER_PATH ((const char*) (gTempBrowserPath))

#define RETRO_MAKE_BROWSER_PATH(N) \
  gTempBrowserPath[0] = 0; \
  strcat(gTempBrowserPath, "assets/"); \
  strcat(gTempBrowserPath, name)

#endif


void* Resource_Load(const char* name, U32* outSize)
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


void  Palette_LoadFromBitmap(const char* name, Palette* palette)
{
  U32 width, height;

  U8* imageData = NULL;

  char n[256];
  n[0] = 0;
  strcat(&n[0], "assets/");
  strcat(&n[0], name);

  #if defined(RETRO_WINDOWS)
    U32 resourceSize = 0;
    void* resourceData = Resource_Load(name, &resourceSize);
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

    if (Palette_HasColour(palette, colour) == false)
    {
      Palette_Add(palette, colour);
    }

  }
}


void Bitmap_LoadPaletted(const char* name, Bitmap* outBitmap, U8 colourOffset)
{
  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);
  
  SDL_Texture* texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

  void* pixelsVoid;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;

  for(U32 i=0, j=0;i < width * height;++i, j+=3)
  {
    U32 idx = (imageData[i] + colourOffset) & 0xFF;
    Colour colour = Palette_GetColour(&gSettings.palette, idx);

    pixels[j+0] = colour.r;
    pixels[j+1] = colour.g;
    pixels[j+2] = colour.b;
  }

  SDL_UnlockTexture(texture);

  outBitmap->w = width;
  outBitmap->h = height;
  outBitmap->handle = texture;
  outBitmap->imageData = imageData;
}

void Bitmap_Load(const char* name, Bitmap* outBitmap, U8 transparentIndex)
{
  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);

  SDL_Texture* texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  void* pixelsVoid;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;

  Palette* palette = &gSettings.palette;
  
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

  outBitmap->w = width;
  outBitmap->h = height;
  outBitmap->handle = texture;
  outBitmap->imageData = imageData;
}

void Sprite_Make(Sprite* inSprite, Bitmap* bitmap, U32 x, U32 y, U32 w, U32 h)
{
  assert(bitmap);
  inSprite->bitmap = bitmap;
  inSprite->rect.x = x;
  inSprite->rect.y = y;
  inSprite->rect.w = w;
  inSprite->rect.h = h;
}

void Retro_Animation_Load(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight, S32 frameOffsetX, S32 frameOffsetY)
{
  assert(numFrames < RETRO_MAX_ANIMATED_SPRITE_FRAMES);
  assert(inAnimatedSprite);
  assert(bitmap);

  inAnimatedSprite->bitmap = bitmap;
  inAnimatedSprite->frameCount = numFrames;
  inAnimatedSprite->frameLength = frameLengthMilliseconds;
  inAnimatedSprite->w = frameWidth;
  inAnimatedSprite->h = frameHeight;

  SDL_Rect frame;
  frame.x = originX;
  frame.y = originY;
  frame.w = frameWidth;
  frame.h = frameHeight;

  for(U8 i=0;i < numFrames;i++)
  {
    inAnimatedSprite->frames[i] = frame;
    frame.x += frameOffsetX;
    frame.y += frameOffsetY;
  }

}

void  Animation_LoadHorizontal(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  Retro_Animation_Load(inAnimatedSprite, bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, frameWidth, 0);
}

void  Animation_LoadVertical(Animation* inAnimatedSprite, Bitmap* bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight)
{
  Retro_Animation_Load(inAnimatedSprite, bitmap, numFrames, frameLengthMilliseconds, originX, originY, frameWidth, frameHeight, 0, frameHeight);
}

Size  Screen_GetSize()
{
  Size size;
  size.w = gSettings.windowWidth;
  size.h = gSettings.windowHeight;
  return size;
}

void Canvas_SetSize(Size size)
{
  gCanvasSize.w = size.w;
  gCanvasSize.h = size.h;
}

S32 Canvas_GetWidth()
{
  return gCanvasSize.w;
}

S32 Canvas_GetHeight()
{
  return gCanvasSize.h;
}

void Canvas_Set(U8 id)
{
  assert(id < RETRO_CANVAS_COUNT);
  gCanvasTexture = gCanvasTextures[id];
  SDL_SetRenderTarget(gRenderer, gCanvasTexture);
}

void Canvas_SetFlags(U8 id, U8 flags, U8 colour)
{
  assert(id < RETRO_CANVAS_COUNT);

  gCanvasFlags[id] = flags;
  gCanvasBackgroundColour[id] = colour;

  if (flags & CNF_Blend)
    SDL_SetTextureBlendMode(gCanvasTextures[id], SDL_BLENDMODE_BLEND);
  else
    SDL_SetTextureBlendMode(gCanvasTextures[id], SDL_BLENDMODE_NONE);

}

void Canvas_Splat(Bitmap* bitmap, S32 x, S32 y, Rect* srcRectangle)
{
  SDL_Rect src, dst;
  SDL_Texture* texture = (SDL_Texture*) bitmap->handle;

  if (srcRectangle == NULL)
  {
    src.x = 0;
    src.y = 0;
    src.w = bitmap->w;
    src.h = bitmap->h;
  }
  else
  {
    src.x = srcRectangle->left;
    src.y = Rect_GetWidth(srcRectangle);
    src.w = srcRectangle->top;
    src.h = Rect_GetHeight(srcRectangle);
  }

  dst.x = x;
  dst.y = y;
  dst.w = src.w;
  dst.h = src.h;

  SDL_RenderCopy(gRenderer, texture, &src, &dst);
}

void  Canvas_Splat2(Bitmap* bitmap, S32 x, S32 y, SDL_Rect* srcRectangle)
{
  assert(srcRectangle);

  SDL_Rect dst;
  SDL_Texture* texture = (SDL_Texture*) bitmap->handle;

  dst.x = x;
  dst.y = y;
  dst.w = srcRectangle->w;
  dst.h = srcRectangle->h;

  SDL_RenderCopy(gRenderer, texture, srcRectangle, &dst);
}

void  Canvas_Splat3(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle)
{
  assert(srcRectangle);

  SDL_Texture* texture = (SDL_Texture*) bitmap->handle;
  SDL_RenderCopy(gRenderer, texture, srcRectangle, dstRectangle);
}

void Canvas_SplatFlip(Bitmap* bitmap, SDL_Rect* dstRectangle, SDL_Rect* srcRectangle, U8 flipFlags)
{
  assert(srcRectangle);

  SDL_Texture* texture = (SDL_Texture*) bitmap->handle;
  SDL_RenderCopyEx(gRenderer, texture, srcRectangle, dstRectangle, 0.0f, NULL, flipFlags);
}

void Canvas_Place(StaticSpriteObject* spriteObject)
{
  assert(spriteObject);
  Canvas_Splat2(spriteObject->sprite->bitmap, spriteObject->x, spriteObject->y, &spriteObject->sprite->rect);
}

void Canvas_Place2(Sprite* sprite, S32 x, S32 y)
{
  assert(sprite);
  Canvas_Splat2(sprite->bitmap, x, y, &sprite->rect);
}

void Canvas_PlaceAnimated(AnimatedSpriteObject* spriteObject, bool updateTiming)
{
  if (updateTiming && (spriteObject->flags & SOF_Animation) != 0)
  {
    spriteObject->frameTime += gDeltaTime;

    if (spriteObject->frameTime >= 1000)
      spriteObject->frameTime = 0; // Prevent spiral out of control.

    while(spriteObject->frameTime > spriteObject->animation->frameLength)
    {
      spriteObject->frameNumber++;
      spriteObject->frameTime -= spriteObject->animation->frameLength;

      if (spriteObject->frameNumber >= spriteObject->animation->frameCount)
      {
        if (spriteObject->flags & SOF_AnimationOnce)
        {
          spriteObject->flags &= ~SOF_Animation;
          spriteObject->frameNumber = spriteObject->animation->frameCount - 1; // Stop
          break;
        }
        else
        {
          spriteObject->frameNumber = 0; // Loop around.
        }
      }
    }
  }

  assert(spriteObject->frameNumber < spriteObject->animation->frameCount);
  Canvas_PlaceAnimated2(spriteObject->animation, spriteObject->x, spriteObject->y, spriteObject->frameNumber, spriteObject->flags & FF_Mask);
}

void Canvas_PlaceAnimated2(Animation* animatedSprite, S32 x, S32 y, U8 frame, U8 flipFlags)
{
  assert(animatedSprite);
  assert(frame < animatedSprite->frameCount);
  SDL_Rect src = animatedSprite->frames[frame];
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = src.w;
  dst.h = src.h;
  Canvas_SplatFlip(animatedSprite->bitmap, &dst, &src, flipFlags & FF_Mask);
}

void Canvas_PlaceScaled(Sprite* sprite, U32 x, U32 y, U32 scale)
{
  assert(sprite);
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = sprite->rect.w * scale;
  dst.h = sprite->rect.h * scale;

  Canvas_Splat3(sprite->bitmap, &dst, &sprite->rect);
}

void Canvas_PlaceScaledF(Sprite* sprite, U32 x, U32 y, float scale)
{
  assert(sprite);
  SDL_Rect dst;
  dst.x = x;
  dst.y = y;
  dst.w = sprite->rect.w * scale;
  dst.h = sprite->rect.h * scale;

  Canvas_Splat3(sprite->bitmap, &dst, &sprite->rect);
}

void Canvas_Flip()
{
  SDL_RenderPresent(gRenderer);
}

void Canvas_Clear()
{
  SDL_RenderClear(gRenderer);
}

void Canvas_ClearColour(U8 colour)
{
  
}

void  Palette_Make(Palette* palette)
{
  assert(palette);
  palette->count = 0;
  palette->fallback = 1;
  palette->transparent = 0;
}

void  Palette_Add(Palette* palette, Colour colour)
{
  assert(palette);
  assert(palette->count <= 255);
  palette->colours[palette->count] = colour;
  ++palette->count;
}

void Palette_AddRGBInt(Palette* palette, U32 rgb)
{
  RetroFourByteUnion c;
  c.q = rgb;
  
  Palette_Add(palette, Colour_Make(c.b[2], c.b[1], c.b[0]));
}

U8 Palette_FindColour(Palette* palette, Colour colour)
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

bool Palette_HasColour(Palette* palette, Colour colour)
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

void Palette_CopyTo(const Palette* src, Palette* dst)
{
  assert(src);
  assert(dst);

  dst->count = src->count;
  dst->fallback = src->fallback;
  dst->transparent = src->transparent;
  memcpy(dst->colours, src->colours, sizeof(src->colours));
}

Colour Colour_Make(U8 r, U8 g, U8 b)
{
  Colour c;
  c.r = r;
  c.g = g;
  c.b = b;
  return c;
}

Colour Colour_ReadRGB(U8* p)
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

U8* Arena_Obtain(U32 size)
{
  assert(gArena.current + size < gArena.end); // Ensure can fit.
  U8* mem = gArena.current;
  gArena.current += size;
  return mem;
}

void Arena_Rewind(U8* mem)
{
  assert(mem >= gArena.begin);
  assert(mem <= gArena.current);
  gArena.current = mem;
}

int  Arena_PctSize()
{
  U32 used = (gArena.current - gArena.begin);
  float pct = ((float) used / (float) RETRO_ARENA_STACK_SIZE);
  return (int) (pct * 100.0f);
}

void Scope_Push(int name)
{
  assert(gScopeStackIndex < 256);

  ++gScopeStackIndex;
  ScopeStack* scope = &gScopeStack[gScopeStackIndex];

  scope->name = name;
  scope->head = NULL;
  scope->offset = gArena.current;
}

int Scope_GetName()
{
  ScopeStack* scope = &gScopeStack[gScopeStackIndex];
  return scope->name;
}

U8* Scope_Obtain(U32 size)
{
  ScopeStack* scope = &gScopeStack[gScopeStackIndex];
  assert(scope->offset + size < gArena.end); // Ensure can fit.
  return Arena_Obtain(size);
}

U8* Scope_ObtainWithFinaliser(U32 size, void(*finaliserFn)(void*))
{
  ScopeStack* scope = &gScopeStack[gScopeStackIndex];
  assert(scope->offset + size < gArena.end); // Ensure can fit.

  U8* mem = Arena_Obtain(sizeof(struct Finaliser) + size);

  struct Finaliser* finaliser = (struct Finaliser*) mem;
  U8* userMem = mem + sizeof(struct Finaliser);
  finaliser->head = scope->head;
  scope->head = finaliser;
  finaliser->fn = finaliserFn;
  finaliser->mem = userMem;

  return userMem;
}

void Scope_Rewind()
{
  ScopeStack* scope = &gScopeStack[gScopeStackIndex];
  
  struct Finaliser* finaliser = scope->head;
  while(finaliser)
  {
    if (finaliser->fn != NULL)
    {
      finaliser->fn(finaliser->mem);
    }

    finaliser = finaliser->head;
  }

  Arena_Rewind(scope->offset);
}

void Scope_Pop()
{
  assert(gScopeStackIndex > 0);
  Scope_Rewind();
  --gScopeStackIndex;
}

void Canvas_DrawPalette(Palette* palette, U32 Y)
{
  int w = Canvas_GetWidth() / 16;
  int h = 8;

  int x = 0;
  int y = 0;

  for(int i=0;i < palette->count;i++)
  {
    Rect rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + w;
    rect.bottom = y + h;

    Canvas_DrawFilledRectangle(i, rect);

    if (i > 0 && i % 16 == 0)
    {
      x = 0;
      y += h;
    }
    else
    {
      x += w;
    }
  }
  
}

void Canvas_DrawRectangle(U8 colour, Rect rect)
{
  Colour rgb = Palette_GetColour(&gSettings.palette, colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  RETRO_SDL_DRAW_PUSH_RGB(t, rgb);

  SDL_RenderDrawRect(gRenderer, &dst);

  RETRO_SDL_DRAW_POP_RGB(t);
}

void Canvas_DrawFilledRectangle(U8 colour, Rect rect)
{
  Colour rgb = Palette_GetColour(&gSettings.palette, colour);
  SDL_Rect dst;
  RETRO_SDL_TO_RECT(rect, dst);

  RETRO_SDL_DRAW_PUSH_RGB(t, rgb);

  SDL_RenderFillRect(gRenderer, &dst);

  RETRO_SDL_DRAW_POP_RGB(t);
}

char* gFmtScratch;

void Canvas_PrintStr(U32 x, U32 y, Font* font, U8 colour, const char* str)
{
  assert(font);
  assert(str);

  Colour rgb = Palette_GetColour(&gSettings.palette, colour);

  SDL_Rect s, d;
  s.x = 0;
  s.y = 0;
  s.w = 0;
  s.h = font->height;
  d.x = x;
  d.y = y;
  d.w = 0;
  d.h = s.h; 

  RETRO_SDL_TEXTURE_PUSH_RGB(t, (SDL_Texture*) font->bitmap.handle, rgb);

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

    SDL_RenderCopy(gRenderer, (SDL_Texture*) font->bitmap.handle, &s, &d);

    d.x += d.w;
  }

  RETRO_SDL_TEXTURE_POP_RGB(t, (SDL_Texture*) font->bitmap.handle);

}

void Canvas_PrintF(U32 x, U32 y, Font* font, U8 colour, const char* fmt, ...)
{
  assert(font);
  assert(fmt);
  va_list args;
  va_start(args, fmt);
  vsprintf(gFmtScratch, fmt, args);
  va_end(args);

  Canvas_PrintStr(x, y, font, colour, gFmtScratch);
}

void AnimatedSpriteObject_Make(AnimatedSpriteObject* inAnimatedSpriteObject, Animation* animation, S32 x, S32 y)
{
  assert(inAnimatedSpriteObject);
  inAnimatedSpriteObject->animation = animation;
  inAnimatedSpriteObject->flags = 0;
  inAnimatedSpriteObject->frameNumber = 0;
  inAnimatedSpriteObject->frameTime = 0;
  inAnimatedSpriteObject->x = x;
  inAnimatedSpriteObject->y = y;
}

void AnimatedSpriteObject_PlayAnimation(AnimatedSpriteObject* inAnimatedSpriteObject, bool playing, bool loop)
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

void AnimatedSpriteObject_SwitchAnimation(AnimatedSpriteObject* animatedSpriteObject, Animation* newAnimation, bool animate)
{
  assert(animatedSpriteObject);
  assert(newAnimation);

  animatedSpriteObject->animation = newAnimation;
  if (animate)
    animatedSpriteObject->flags |= SOF_Animation;
  else
    animatedSpriteObject->flags &= ~SOF_Animation;
  animatedSpriteObject->frameNumber = 0;
}

void  Canvas_Debug(Font* font)
{
  assert(font);
  RetroFourByteUnion f;
  f.q = Scope_GetName();

  Canvas_PrintF(0, Canvas_GetHeight() - font->height, font, 1, "Scope=%c%c%c%c Mem=%i%% FPS=%.2g Dt=%i", f.b[3], f.b[2], f.b[1], f.b[0], Arena_PctSize(), gFps, gDeltaTime);
}

void  Font_Make(Font* font)
{
  assert(font);
  memset(font->widths, 0, sizeof(font->widths));
  font->height = 0;
  font->bitmap.w = 0;
  font->bitmap.h = 0;
  font->bitmap.handle = NULL;
  font->bitmap.imageData = NULL;
}

void Font_Load(const char* name, Font* outFont, Colour markerColour, Colour transparentColour)
{
  RETRO_UNUSED(outFont);
  RETRO_UNUSED(markerColour);
  RETRO_UNUSED(transparentColour);

  U32 width, height;

  U8* imageData = NULL;

#ifdef RETRO_WINDOWS
  U32 resourceSize = 0;
  void* resourceData = Resource_Load(name, &resourceSize);
  lodepng_decode_memory(&imageData, &width, &height, resourceData, resourceSize, LCT_RGB, 8);
#elif defined(RETRO_BROWSER)
  RETRO_MAKE_BROWSER_PATH(name);
  lodepng_decode_file(&imageData, &width, &height, RETRO_BROWSER_PATH, LCT_RGB, 8);
#endif

  assert(imageData);

  SDL_Texture* texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height - 1);

  void* pixelsVoid;
  int pitch;

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_LockTexture(texture, NULL, &pixelsVoid, &pitch);
  U8* pixels = (U8*) pixelsVoid;
  
  U32 i,j;

  U32 lx = 0xCAFEBEEF;
  U8  ch = '!';

  // Scan the first line for markers.
  for(i=0;i < width * 3;i+=3)
  {
    Colour col = Colour_ReadRGB(&imageData[i]);
    if (Colour_Equals(col, markerColour))
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
        lx = x;
      }
    }
  }

  outFont->widths[' '] = outFont->widths['e'];

  // Copy rest of image into the texture.
  for(i=0, j=width * 3;i < width * height * 4;i+=4, j+=3)
  {
    Colour col = Colour_ReadRGB(&imageData[j]);

    pixels[i+0] = 0xFF;
    pixels[i+1] = 0xFF;
    pixels[i+2] = 0xFF;

    if (Colour_Equals(col, transparentColour))
    {
      pixels[i+3] = 0x00;
    }
    else
    {
      pixels[i+3] = 0xFF;
    }
  }

  SDL_UnlockTexture(texture);

  outFont->height = height - 1;
  outFont->bitmap.w = width;
  outFont->bitmap.h = height - 1;
  outFont->bitmap.handle = texture;
  outFont->bitmap.imageData = imageData;
}

int Input_TextInput(char* str, U32 capacity)
{
  assert(str);
  U32 len = strlen(str);

  switch(gInputCharState)
  {
    default:
    case ICS_None:
    return 0;
    case ICS_Character:
    {
      if (len < capacity)
      {
        str[len] = gInputChar;
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

InputActionBinding* Input_GetAction(int action)
{
  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    InputActionBinding* binding = &gInputActions[i];
    if (binding->action == action)
      return binding;
  }
  return NULL;
}

InputActionBinding* Input_MakeAction(int action)
{
  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    InputActionBinding* binding = &gInputActions[i];
    if (binding->action == 0xDEADBEEF)
    {
      binding->action = action;
      return binding;
    }
  }
  assert(true); // Ran out of InputActionBindings
  return NULL;
}


void  Input_BindKey(int key, int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  
  if (binding == NULL)
  {
    binding = Input_MakeAction(action);
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

void  Input_BindAxis(int axis, int action)
{
  InputActionBinding* binding = Input_GetAction(action);

  if (binding == NULL)
  {
    binding = Input_MakeAction(action);
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

bool  Input_GetActionDown(int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  assert(binding);

  return binding->state == 1;
}

bool  Input_GetActionReleased(int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  assert(binding);

  return binding->state == 0 && binding->lastState == 1;
}

bool  Input_GetActionPressed(int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  assert(binding);

  return binding->state == 1 && binding->lastState == 0;
}

S16   Input_GetActionNowAxis(int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  assert(binding);

  return binding->state;
}

S16   Input_GetActionDeltaAxis(int action)
{
  InputActionBinding* binding = Input_GetAction(action);
  assert(binding);

  return binding->state - binding->lastState;
}

typedef enum
{
  TF_None    = 0,
  TF_Started = 1,
  TF_Paused  = 2
} Retro_TimerFlags;

void  Timer_Make(Timer* timer)
{
  assert(timer);
  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Timer_Start(Timer* timer)
{
  assert(timer);
  timer->flags = TF_Started;
  timer->start = SDL_GetTicks();
  timer->paused = 0;
}

void  Timer_Stop(Timer* timer)
{
  assert(timer);

  timer->start = 0;
  timer->paused = 0;
  timer->flags = TF_None;
}

void  Timer_Pause(Timer* timer)
{
  assert(timer);

  if (timer->flags == TF_Started)
  {
    timer->flags |= TF_Paused;
    timer->paused = SDL_GetTicks() - timer->start;
    timer->start = 0;
  }
}

void  Timer_Unpause(Timer* timer)
{
  assert(timer);

  if (timer->flags == 3 /* Started | Paused */)
  {
    timer->flags = TF_Started; // &= ~Paused
    timer->start = SDL_GetTicks() - timer->paused;
    timer->paused = 0;
  }
}

U32   Timer_GetTicks(Timer* timer)
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

bool  Timer_IsStarted(Timer* timer)
{
  return timer->flags >= TF_Started;
}

bool  Timer_IsPaused(Timer* timer)
{
  return timer->flags >= TF_Paused;
}

void Restart()
{
  gArena.current = gArena.begin;
  
  gScopeStackIndex = 0;
  gScopeStack[0].head = NULL;
  gScopeStack[0].offset = gArena.current;
  gScopeStack[0].name = 'INIT';

  Start();
}

void Frame()
{

  Timer_Start(&gCapTimer);

  gDeltaTime = Timer_GetTicks(&gDeltaTimer);

  SDL_Event event;
  gInputCharState = ICS_None;

  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
      {
        gQuit = true;
      }
      break;
      case SDL_TEXTINPUT:
      {
        gInputChar = event.text.text[0];
        gInputCharState = ICS_Character;
      }
      break;
      case SDL_KEYDOWN:
      {

        if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
          gInputCharState = ICS_Backspace;
        }
        else if (event.key.keysym.sym == SDLK_RETURN)
        {
          gInputCharState = ICS_Enter;
        }
      }
      break;
    }
  }

  gFps = gCountedFrames / (Timer_GetTicks(&gFpsTimer) / 1000.0f);
  if (gFps > 200000.0f)
  {
    gFps = 0.0f;
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  for (U32 i=0;i < RETRO_MAX_INPUT_ACTIONS;i++)
  {
    InputActionBinding* binding = &gInputActions[i];
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
    if (gCanvasFlags[i] & CNF_Clear)
    {
      Canvas_Set(i);
      Colour col = Palette_GetColour(&gSettings.palette, gCanvasBackgroundColour[i]);
      SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, 0x00);
      Canvas_Clear();
      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x00);
    }
  }

  Canvas_Set(0);
  
  Step();
  SDL_SetRenderTarget(gRenderer, NULL);

  for (int i=0;i < RETRO_CANVAS_COUNT;i++)
  {
    if (gCanvasFlags[i] & CNF_Render)
    {
      SDL_RenderCopy(gRenderer, gCanvasTextures[i], NULL, NULL);
    }
  }

  Canvas_Flip();
  
  ++gCountedFrames;
  
  Timer_Start(&gDeltaTimer);
}


#ifdef RETRO_WINDOWS
int main(int argc, char *argv[])
#endif
#ifdef RETRO_BROWSER
int main(int argc, char **argv)
#endif
{

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  gArena.begin = malloc(RETRO_ARENA_STACK_SIZE);
  gArena.current = gArena.begin;
  gArena.end = gArena.begin + RETRO_ARENA_STACK_SIZE;

  memset(gArena.begin, 0, RETRO_ARENA_STACK_SIZE);

  gFmtScratch = malloc(1024);

  memset(gInputActions, 0, sizeof(gInputActions));

  for(int i=0;i < RETRO_MAX_INPUT_ACTIONS;++i)
  {
    gInputActions[i].action = 0xDEADBEEF;
  }

  gSettings.windowWidth = RETRO_WINDOW_DEFAULT_WIDTH;
  gSettings.windowHeight = RETRO_WINDOW_DEFAULT_HEIGHT;
  
  Palette_Make(&gSettings.palette);

  for (U32 i=0;i < RETRO_ARRAY_COUNT(kDefaultPalette);i++)
    Palette_Add(&gSettings.palette, kDefaultPalette[i]);

  gWindow = SDL_CreateWindow( 
    RETRO_WINDOW_CAPTION,
    SDL_WINDOWPOS_UNDEFINED, 
    SDL_WINDOWPOS_UNDEFINED,
    gSettings.windowWidth,
    gSettings.windowHeight,
    SDL_WINDOW_SHOWN
  );

  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  Init(&gSettings);

  gCanvasSize = Size_Make(RETRO_CANVAS_DEFAULT_WIDTH, RETRO_CANVAS_DEFAULT_HEIGHT);

  for (U8 i=0;i < RETRO_CANVAS_COUNT;i++)
  {
    gCanvasTextures[i] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, gCanvasSize.w, gCanvasSize.h);
    int flags = CNF_Clear | CNF_Render;
    if (i > 0)
      flags |= CNF_Blend;

    Canvas_SetFlags(i, flags, 0);
  }

  Canvas_Set(0);

  gQuit = false;

  Restart();

  gCountedFrames = 0;
  Timer_Start(&gFpsTimer);
  Timer_Start(&gDeltaTimer);

  #ifdef RETRO_WINDOWS

  while(gQuit == false)
  {
    Frame();
    float frameTicks = Timer_GetTicks(&gCapTimer);
    if (frameTicks < (1000.0f / RETRO_FRAME_RATE))
    {
      SDL_Delay((1000.0f / RETRO_FRAME_RATE) - frameTicks);
    }
  }

  #endif

  #ifdef RETRO_BROWSER

  emscripten_set_main_loop(Frame, RETRO_FRAME_RATE, 1);

  #endif

  free(gArena.begin);
  SDL_Quit();
  return 0;
}

#undef RETRO_SDL_DRAW_PUSH_RGB
#undef RETRO_SDL_DRAW_POP_RGB
#undef RETRO_SDL_TO_RECT

#endif