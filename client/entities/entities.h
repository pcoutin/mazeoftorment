#ifndef _MOT_ENTITIES_H
#define _MOT_ENTITIES_H

typedef struct _mot_picture PICTURE;
typedef struct _mot_player PLAYER;

struct _mot_picture
{
   SDL_Texture *texture;
   SDL_Rect rect;
};

struct _mot_player
{
   char *name;
   PICTURE *sprite;

   short x, y;                // location in maze tiles

   unsigned char type;        // 0 is prey, 1 is predator
   unsigned char playerno;
   unsigned char dead;

   PLAYER *next;
   PLAYER *prev;

};

void init_player(TCPsocket sock, PLAYER *player);

void drawPlayer(PLAYER *);

void local_player_update(TCPsocket sock, PLAYER *, PLAYER *, const Uint8 *);

void movePlayer(PLAYER *p, short newx, short newy);

void clearPlayer(PLAYER *);


/*
 * GLOBALS
 */

// Black tile to erase players as they move, and hunter/prey sprites.
PICTURE black, psprite, hsprite;

#endif
