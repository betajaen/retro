#ifndef RETRO_SHORTHAND_H
#define RETRO_SHORTHAND_H

typedef Retro_SpriteHandle           SpriteHandle;
typedef Retro_AnimationHandle        AnimationHandle;
typedef Retro_BitmapHandle           BitmapHandle;
typedef Retro_SoundHandle            SoundHandle;
typedef Retro_Rect                   Rect;
typedef Retro_Colour                 Colour;
typedef Retro_Palette                Palette;
typedef Retro_Font                   Font;
typedef Retro_Timer                  Timer;
typedef Retro_SpriteObjectFlags      SpriteObjectFlags;
typedef Retro_SpriteObject           SpriteObject;
typedef Retro_AnimationObject        AnimationObject;

#define   Hex_Char             Retro_Hex_Char             
#define   Hex_U8               Retro_Hex_U8
#define   Hex_S8               Retro_Hex_S8
#define   Hex_U16              Retro_Hex_U16
#define   Hex_S16              Retro_Hex_S16
#define   Hex_U32              Retro_Hex_U32
#define   Hex_S32              Retro_Hex_S32
#define   Hex_Colour           Retro_Hex_Colour
#define   Colour_Make          Retro_Colour_Make
#define   Point_Make           Retro_Point_Make
#define   Size_Make            Retro_Size_Make
#define   Rect_Make            Retro_Rect_Make

#   define Arena_LoadFromMemory       Retro_Arena_LoadFromMemory
#   define Arena_Load                 Retro_Arena_Load
#   define Arena_SaveToMemory         Retro_Arena_SaveToMemory
#   define Arena_Save                 Retro_Arena_Save

#   define Scope_Push              Retro_Scope_Push
#   define Scope_Name              Retro_Scope_Name
#   define Scope_Obtain            Retro_Scope_Obtain
#   define Scope_Rewind            Retro_Scope_Rewind
#   define Scope_Pop               Retro_Scope_Pop

#  define  Resources_LoadPalette      Retro_Resources_LoadPalette
#  define  Resources_LoadBitmap       Retro_Resources_LoadBitmap
#  define  Resources_LoadSound        Retro_Resources_LoadSound
#  define  Resources_Font_Load        Retro_Resources_LoadFont


#   define Sprites_LoadSprite              Retro_Sprites_LoadSprite
#   define Sprites_LoadAnimationH          Retro_Sprites_LoadAnimationH
#   define Sprites_LoadAnimationV          Retro_Sprites_LoadAnimationV
#   define Sprites_MakeAnimationObject     Retro_Sprites_MakeAnimationObject
#   define Sprites_PlayAnimationObject     Retro_Sprites_PlayAnimationObject
#   define Sprites_SetAnimationObject      Retro_Sprites_SetAnimationObject



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


#   define    Audio_PlaySound              Retro_Audio_PlaySound
#   define    Audio_ClearSounds            Retro_Audio_ClearSounds
#   define    Audio_PlayMusic              Retro_Audio_PlayMusic
#   define    Audio_StopMusic              Retro_Audio_StopMusic


#   define  Palette_Add            Retro_Palette_Add
#   define  Palette_Add2           Retro_Palette_Add2
#   define  Palette_AddRGB         Retro_Palette_AddARGB
#   define  Palette_Index          Retro_Palette_Index
#   define  Palette_Has            Retro_Palette_Has
#   define  Palette_Get            Retro_Palette_Get


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



#   define Timer_Make(TIMER)      Retro_TimerMake(TIMER)
#   define Timer_Start(TIMER)     Retro_Timer_Start(TIMER)
#   define Timer_Stop(TIMER)      Retro_Timer_Stop(TIMER)
#   define Timer_Pause(TIMER)     Retro_Timer_Pause(TIMER)
#   define Timer_Unpause(TIMER)   Retro_Timer_Unpause(TIMER)
#   define Timer_Ticks(TIMER)     Retro_Timer_Ticks(TIMER)
#   define Timer_Started(TIMER)   Retro_Timer_Started(TIMER)
#   define Timer_Paused(TIMER)    Retro_Timer_Paused(TIMER)

#   define _RGB(STR)           Hex_Colour(STR)
#   define _POINT(X, Y)        Point_Make(X, Y)
#   define _RECT(X, Y, W, H)   Rect_Make(X, Y, W, H)
#   define _SIZE(W, H)         Size_Make(W, H)
#   define _KILOBYTES(BYTES)   RETRO_KILOBYTES(BYTES)
#   define _MEGABYTES(BYTES)   RETRO_MEGABYTES(BYTES)


#endif

