#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* debug */
#include "../common/mot_maze.h"

#include "mot.h"

PICTURE
loadPic(char *path)
{
   SDL_Surface *s;
   SDL_Texture *tex;
   PICTURE pic;

   s = IMG_Load(path);
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
   SDL_Window *window;
   PICTURE face;
   CLC_CONFIG config;

   /* debug, should remove later */
   FILE *mfile;

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

   int linx = 42, liny = 42;
   face = loadPic("img/test.gif");

   /*
    * debug load maze from a file
    */
   mfile = fopen("maze.dat", "r");
   fread(&MAZE, sizeof(MAZE), 1, mfile);
   MAZE.data = malloc(MAZE.size * sizeof(MCELL));
   fread(MAZE.data, sizeof(MCELL), MAZE.size, mfile);
   fclose(mfile);

   for (;;)
   {
      SDL_Event e;
      if (SDL_WaitEvent(&e)) {
         if (e.type == SDL_QUIT) {
            break;
         }
      }

      /* Clear last frame. */
      SDL_RenderClear(renderer);

      if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
         SDL_GetMouseState(&linx, &liny);
      }

      /*
       * Draw things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

      drawPic(face, linx, liny);

      /* Draw it in the middle of the screen! */
      draw_maze((config.win_width - MAZE.w * 16) / 2,
            (config.win_height - MAZE.h * 16) / 2);

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
