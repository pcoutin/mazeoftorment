#ifndef _CLC_H_
#define _CLC_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define CFG_FNAME    "clc.cfg"

#define DEF_WIDTH    	800
#define DEF_HEIGHT   	600
#define DEF_FULLSCREEN	0

SDL_Renderer *renderer;

typedef struct
{
   unsigned int win_width;
   unsigned int win_height;
   unsigned char win_fullscreen;
} CLC_CONFIG;

typedef struct
{
   SDL_Texture *texture;
   SDL_Rect rect;
} PICTURE;

#endif
