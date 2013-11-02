#include "../mot.h"

/*
 * initx and inity are in terms of the maze cells, not pixels.
 */
void
init_player(PLAYER *player, short initx,
      short inity,
      unsigned char ishunter,
      PICTURE *sprite)
{
   player->x = MAZE.X + TILE_WIDTH * initx + 10;
   player->y = MAZE.Y + TILE_HEIGHT * inity + 10;
   player->type = ishunter;
   player->sprite = sprite;
}

/*
 * Update the user of this client (not the remote players).
 * reimplement later by asking the server if we can move and then moving?
 * also use higher level functions to move by one cell somewhere
 * i mean what
 */
void
local_player_update(PLAYER *me, const Uint8 *kbdstate)
{
   int x, y;

   /* Find the player's coordinates in maze row/column */
   x = (me->x - (int) MAZE.X + 16) / TILE_WIDTH;
   y = (me->y - (int) MAZE.Y + 16) / TILE_HEIGHT;

#ifdef _DEBUG
   printf("(%i %i) %x\n", x, y, mazetile(x + 1, y + 1));
   printf("(me->x = %d  - MAZE.X = %d + 16) / %d = %d\n",
         me->x, MAZE.X, TILE_WIDTH,
         x);
#endif

   /* Update based on walls... */
   if (!mazetile(x, y + 1) && kbdstate[SDL_SCANCODE_DOWN])
   {
      me->y += TILE_HEIGHT * 2;
   }
   else if (!mazetile(x, y - 1) && kbdstate[SDL_SCANCODE_UP])
   {
      me->y -= TILE_HEIGHT * 2;
   }
   else if (!mazetile(x + 1, y) && kbdstate[SDL_SCANCODE_RIGHT])
   {
      me->x += TILE_WIDTH * 2;
   }
   else if (!mazetile(x - 1, y) && kbdstate[SDL_SCANCODE_LEFT])
   {
      me->x -= TILE_WIDTH * 2;
   }
}

void
drawPlayer(PLAYER *player)
{
}
