#ifndef _MOT_H_
#define _MOT_H_

#define CFG_FNAME       "config.ini"

#define DEF_WIDTH    	800
#define DEF_HEIGHT   	600
#define DEF_FULLSCREEN	0
#define DEF_HWACCEL     1
#define MAX_PLAYERNUM   32

/* For the maze. */
#define TILE_WIDTH      16
#define TILE_HEIGHT     16

/* GLOBALS */
SDL_Renderer *renderer;

typedef struct _clc_config CLC_CONFIG;

struct _clc_config
{
   int win_width;
   int win_height;
   Uint32 win_flags;
   Uint32 renderflags;
   char *defaultsrv;
   char *defaultport;
};


/* cfg.c */
void parsecfg(CLC_CONFIG *config);

/* entities/maze.c */
void draw_maze(int x, int y);
unsigned char mazetile(int x, int y);

/* entities/player.c */

/* entities/picture.c */
PICTURE loadPic(char *path);
void drawPic(PICTURE pic, int x, int y);

#endif
