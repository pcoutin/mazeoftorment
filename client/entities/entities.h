#ifndef _MOT_ENTITIES_H
#define _MOT_ENTITIES_H

typedef struct
{
   SDL_Texture *texture;
   SDL_Rect rect;

} PICTURE;

PICTURE black;

typedef struct _mot_player
{
   char *name;
   PICTURE *sprite;

   short x, y;                // location in maze tiles

   unsigned char type;        // 0 is prey, 1 is predator
   unsigned char playerno;
   unsigned char dead;

} PLAYER;

void init_localplayer(PLAYER *, short, short, unsigned char,
      unsigned char, PICTURE *, char *);
void drawPlayer(PLAYER *);
void local_player_update(PLAYER *, PLAYER *, const Uint8 *);
void clearPlayer(PLAYER *);

#endif
