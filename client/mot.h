#ifndef _MOT_H_
#define _MOT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../common/mot_maze.h"

#define CFG_FNAME       "config.ini"

#define DEF_WIDTH    	800
#define DEF_HEIGHT   	600
#define DEF_FULLSCREEN	0
#define DEF_HWACCEL     1
#define MAX_PLAYERNUM   8

/* For the maze. */
#define TILE_WIDTH      16
#define TILE_HEIGHT     16

SDL_Renderer *renderer;

typedef struct
{
   int win_width;
   int win_height;
   Uint32 win_flags;
   Uint32 renderflags;
   char *defaultsrv;
   char *defaultport;
} CLC_CONFIG;


typedef struct
{
   SDL_Texture *texture;
   SDL_Rect rect;
} PICTURE;

typedef struct
{
   /* The location of the player in pixels. */
   unsigned int x, y;

   /* hunter or prey? 0 is prey, 1 is hunter */
   unsigned char type;
} PLAYER;

/* cfg.c */
void parsecfg(CLC_CONFIG *config);

/* entities/maze.c */
void draw_maze(int x, int y);
MCELL mazetile(int x, int y);

/* entities/player.c */
void update_me(PLAYER *me, const Uint8 *kbdstate);

#endif
