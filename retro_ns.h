#ifndef RETRO_NAMESPACES_H
#define RETRO_NAMESPACES_H

#include "retro.h"


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



const struct RETRO_Arena
{
  void (*loadFromMemory)(U8* mem, bool loadMusic);
  void (*load)(const char* filename, bool loadMusic);
  U8*  (*saveToMemory)(U32* outSize);
  void (*save)(const char* filename);
}
RETRO_ARENA_NAMESPACE_NAME
= {
  Retro_Arena_LoadFromMemory,
  Retro_Arena_Load,
  Retro_Arena_SaveToMemory,
  Retro_Arena_Save
};





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


const struct RETRO_Resources
{
  void         (*loadPalette)(const char* name);
  Retro_BitmapHandle (*loadBitmap)(const char* name, U8 transparentIndex);
  Retro_SoundHandle  (*loadSound)(const char* name);
  void         (*loadFont)(const char* name, Retro_Font* inFont, Retro_Colour markerColour, Retro_Colour transparentColour);
  void         (*loadFontFixed)(const char* name, Retro_Font* inFont, U8 w, Retro_Colour transparentColour);
}
RETRO_RESOURCES_NAMESPACE_NAME
= {
  Retro_Resources_LoadPalette,
  Retro_Resources_LoadBitmap,
  Retro_Resources_LoadSound,
  Retro_Resources_LoadFont,
  Retro_Resources_LoadFontFixed
};




const struct RETRO_Sprites
{
  Retro_SpriteHandle (*loadSprite)(Retro_BitmapHandle bitmap, U32 x, U32 y, U32 w, U32 h);
  Retro_AnimationHandle (*loadAnimationH)(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);
  Retro_AnimationHandle (*loadAnimationV)(Retro_BitmapHandle bitmap, U8 numFrames, U8 frameLengthMilliseconds, U32 originX, U32 originY, U32 frameWidth, U32 frameHeight);
  void (*newAnimation)(Retro_AnimationObject* inAnimatedSpriteObject, Retro_AnimationHandle animation, S32 x, S32 y);
  void (*playAnimation)(Retro_AnimationObject* animatedSpriteObject, bool playing, bool loop);
  void (*setAnimation)(Retro_AnimationObject* animatedSpriteObject, Retro_AnimationHandle newAnimation, bool animate);
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



const struct RETRO_Canvas
{
  S32  (*width)();
  S32  (*height)();
  void (*use)(U8 canvasIndex);
  void (*flags)(U8 id, U8 flags, U8 clearColour);
  void (*clear)();
  void (*copy)(Retro_BitmapHandle bitmap, Retro_Rect* dstRectangle, Retro_Rect* srcRectangle, U8 copyFlags);
  void (*copy2)(Retro_BitmapHandle bitmap, S32 dstX, S32 dstY, S32 srcX, S32 srcY, S32 w, S32 h, U8 copyFlags);
  void (*sprite)(Retro_SpriteObject* spriteObject);
  void (*animate)(Retro_AnimationObject* animationObject, bool updateTiming);
  void (*drawPalette)(S32 y);
  void (*drawPalette2)(S32 y, U8 from, U8 to);
  void (*drawBox)(U8 colour, Retro_Rect rect);
  void (*drawRectangle)(U8 colour, Retro_Rect rect);
  void (*drawSprite)(Retro_SpriteHandle sprite, S32 x, S32 y, U8 flipFlags);
  void (*drawAnimation)(Retro_AnimationHandle animationHandle, S32 x, S32 y, U8 frame, U8 copyFlags);
  void (*print)(S32 x, S32 y, Retro_Font* font, U8 colour, const char* str);
  void (*printf)(S32 x, S32 y, Retro_Font* font, U8 colour, const char* fmt, ...);
  void (*monoPrint)(S32 x, S32 y, S32 w, Retro_Font* font, U8 colour, const char* str);
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




const struct RETRO_Audio
{
  void (*playSound)(Retro_SoundHandle sound, U8 volume);
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


const struct RETRO_Palette {
  void   (*add)(Retro_Colour colour);
  void   (*addRGB)(U8 r, U8 g, U8 b);
  U8     (*index)(Retro_Colour colour);
  bool   (*has)(Retro_Colour colour);
  Retro_Colour (*get)(U8 index);
  void   (*set)(U8 index, Retro_Colour colour);
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


const struct RETRO_Input {
  int(*textInput)(char* str, U32 capacity);
  void(*bindKey)(int retroKey, int action);
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




const struct RETRO_Timer {
  void (*make)(Retro_Timer* timer);
  void (*start)(Retro_Timer* timer);
  void (*stop)(Retro_Timer* timer);
  void (*pause)(Retro_Timer* timer);
  void (*unpause)(Retro_Timer* timer);
  U32  (*ticks)(Retro_Timer* timer);
  bool (*started)(Retro_Timer* timer);
  bool (*paused)(Retro_Timer* timer);
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
