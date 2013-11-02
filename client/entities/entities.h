#ifndef _MOT_ENTITIES_H
#define _MOT_ENTITIES_H

typedef struct
{
   SDL_Texture *texture;
   SDL_Rect rect;

} PICTURE;

typedef struct _mot_player
{
   /* The location of the player in maze tiles. */
   short x, y;

   /* hunter or prey? 0 is prey, 1 is hunter */
   unsigned char type;

   PICTURE *sprite;

} PLAYER;

void init_player(PLAYER *, short, short, unsigned char, PICTURE *);
void drawPlayer(PLAYER *);

#endif
