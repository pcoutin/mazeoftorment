#ifndef _MOT_H_
#define _MOT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../common/mot_maze.h"

#include "entities/entities.h"

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


/* cfg.c */
void parsecfg(CLC_CONFIG *config);

/* entities/maze.c */
void draw_maze(int x, int y);
MCELL mazetile(int x, int y);

/* entities/player.c */
void local_player_update(PLAYER *me, const Uint8 *kbdstate);

/* entities/picture.c */
PICTURE loadPic(char *path);
void drawPic(PICTURE pic, int x, int y);

#endif
