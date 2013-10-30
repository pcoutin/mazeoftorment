#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "mot.h"

PICTURE
loadPic(char *path)
{
   SDL_Surface *s;
   SDL_Texture *tex;
   PICTURE pic;

   if ((s = IMG_Load(path)) == NULL)
   {
      fprintf(stderr, "Failed to load image\n");
      exit(1);
   }

   tex = SDL_CreateTextureFromSurface(renderer, s);
   pic.rect.x = 0;
   pic.rect.y = 0;
   pic.rect.w = s->w;
   pic.rect.h = s->h;
   pic.texture = tex;

   SDL_FreeSurface(s);

   return pic;
}

void
drawPic(PICTURE pic, int x, int y)
{
   pic.rect.x = x;
   pic.rect.y = y;
   SDL_RenderCopy(renderer, pic.texture, NULL, &pic.rect);
}

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

//   int linx = 42, liny = 42;
   face = loadPic("img/test.gif");

   /*
    * debug load maze from a file
    */
   mfile = fopen("maze.dat", "r");
   fread(&MAZE, sizeof(MAZE), 1, mfile);
   MAZE.data = malloc(MAZE.size * sizeof(MCELL));
   fread(MAZE.data, sizeof(MCELL), MAZE.size, mfile);
   fclose(mfile);

   me.x = (config.win_width - MAZE.w * 16) / 2 + 8;
   me.y = (config.win_height - MAZE.h * 16) / 2 + 8;
   me.type = 0; /* FOR NOW */

   /*
    * Initialize maze.
    */
   MAZE.X = (config.win_width - MAZE.w * 16) / 2;
   MAZE.Y = (config.win_height - MAZE.h * 16) / 2;

   /*
    * Game loop!
    */
   for (;;)
   {
      if (SDL_PollEvent(&e))
      {
         if (e.type == SDL_QUIT)
         {
            break;
         }
         //else if (e.type == SDL_KEYDOWN)
         {
            kbdstate = (Uint8 *) SDL_GetKeyboardState(NULL);

            if (kbdstate[SDL_SCANCODE_Q])
            {
               break;
            }

            update_me(&me, SDL_GetKeyboardState(NULL));
         }
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

      drawPic(face, me.x, me.y);

      /* Draw it in the middle of the screen! */
      draw_maze(MAZE.X, MAZE.Y);

      /*
       * Stop drawing things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderPresent(renderer);
   }

   SDL_DestroyTexture(face.texture);

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
