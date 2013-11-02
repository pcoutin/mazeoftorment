#include <stddef.h>
#include "../mot.h"

static void
draw_mazetile(MCELL tile, int x, int y)
{
   if (tile & N_WALL)
   {
      SDL_RenderDrawLine(
            renderer,
            x + TILE_WIDTH/2,
            y,
            x + TILE_WIDTH/2,
            y + TILE_HEIGHT/2
      );
   }
   if (tile & S_WALL)
   {
      SDL_RenderDrawLine(
            renderer,
            x + TILE_WIDTH/2,
            y + TILE_HEIGHT/2,
            x + TILE_WIDTH/2,
            y + TILE_HEIGHT
      );
   }
   if (tile & E_WALL)
   {
      SDL_RenderDrawLine(
            renderer,
            x + TILE_WIDTH/2,
            y + TILE_HEIGHT/2,
            x + TILE_WIDTH,
            y + TILE_HEIGHT/2
      );
   }
   if (tile & W_WALL)
   {
      SDL_RenderDrawLine(
            renderer,
            x,
            y + TILE_HEIGHT/2,
            x + TILE_WIDTH/2,
            y + TILE_HEIGHT/2
      );
   }

}

void
draw_maze(int x, int y)
{
   unsigned int i;

   for (i = 0; i < MAZE.size; i++)
   {
      draw_mazetile(
            *(MAZE.data + i),
            x + (i % MAZE.w) * TILE_WIDTH,
            y + (i / MAZE.w) * TILE_HEIGHT
      );
   }

   /*
    * Make MAZE.width and MAZE.height in pixels
    */
   MAZE.width = TILE_WIDTH * MAZE.w;
   MAZE.height = TILE_HEIGHT * MAZE.h;

   /*
    * Make MAZE.X and MAZE.Y the starting X and Y coordinates of the
    * maze...
    */
   MAZE.X = x;
   MAZE.Y = y;

}

MCELL
mazetile(int x, int y)
{
   MCELL *ret = MAZE.data + x + (MAZE.w * y);

   if (ret - MAZE.data > MAZE.size || ret - MAZE.data < 0)
   {
      return 0; /* Everything outside the maze is EMPTY. */
   }
   return *ret;
}
