#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "mot.h"

int
main(int argc, char *argv[])
{
   SDL_Window  *window;
   PICTURE     face;
   CLC_CONFIG  config;
   Uint8       *kbdstate;
   SDL_Event   e;
   PLAYER      me;
   PLAYER      *remote;
   Uint32 time;

   /* debug, should remove later */
   FILE        *mfile;

   parsecfg(&config);

   window = SDL_CreateWindow(
         "MAZE OF TORMENT",
         SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         config.win_width,
         config.win_height,
         config.win_flags
   );

   SDL_GetWindowSize(window, &config.win_width, &config.win_height);

   if (window == NULL)
   {
      fprintf(stderr, "Could not create window: %s\n",
            SDL_GetError());
      return 1;
   }

   renderer = SDL_CreateRenderer(window, -1, config.renderflags);

   face = loadPic("img/test.gif");

   /*
    * debug load maze from a file
    */
   mfile = fopen("maze.dat", "r");
   fread(&MAZE, sizeof(MAZE), 1, mfile);
   MAZE.data = malloc(MAZE.size * sizeof(MCELL));
   fread(MAZE.data, sizeof(MCELL), MAZE.size, mfile);
   fclose(mfile);

   /*
    * Initialize maze.
    */
   MAZE.X = (config.win_width - MAZE.w * 16) / 2;
   MAZE.Y = (config.win_height - MAZE.h * 16) / 2;

   /* Type is 0 (not hunter :( )... FOR NOW */
   init_player(&me, 6, 6, 0, &face);

   /*
    * Game loop!
    */
   for (;;)
   {
      time = SDL_GetTicks();

      if (SDL_WaitEvent(&e))
      {
         if (e.type == SDL_QUIT)
         {
            break;
         }

         kbdstate = (Uint8 *) SDL_GetKeyboardState(NULL);

         if (kbdstate[SDL_SCANCODE_Q])
         {
            break;
         }

         local_player_update(&me, SDL_GetKeyboardState(NULL));
      }

      /* Clear last frame. */
      SDL_RenderClear(renderer);

      /*
      if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
         SDL_GetMouseState(&linx, &liny);
      }
      */

      /*
       * Draw things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

      printf("%d %d - %d %d\n", me.x, me.y);
      drawPic(face, me.x, me.y);

      /* Draw it in the middle of the screen! */
      draw_maze(MAZE.X, MAZE.Y);

      /*
       * Stop drawing things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderPresent(renderer);

      if (20 > (SDL_GetTicks() - time))
      {
         SDL_Delay(20 - (SDL_GetTicks() - time));
      }
   }

   SDL_DestroyTexture(face.texture);

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
