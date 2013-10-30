#include "../mot.h"

/*
 * Update the user of this client (not the remote players).
 */
void
update_me(PLAYER *me, const Uint8 *kbdstate)
{
   int x, y;

   /* Find the player's coordinates in maze row/column */
   x = (me->x - MAZE.X + 16) / TILE_WIDTH;
   y = (me->y - MAZE.Y + 16) / TILE_HEIGHT;

   printf("(%d %d) %x\n", x, y, mazetile(x + 1, y + 1));

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
