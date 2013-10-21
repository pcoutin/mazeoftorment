#include <stdio.h>
#include <stdlib.h>

#include "clc.h"

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
   CLC_CONFIG config;
   SDL_Window *window;
   PICTURE face;

   parsecfg("config.cfg", &config);

   window = SDL_CreateWindow(
         "box fun",
         SDL_WINDOWPOS_UNDEFINED,
         SDL_WINDOWPOS_UNDEFINED,
         config.win_width,
         config.win_height,
         0
   );

   if (window == NULL)
   {
      fprintf(stderr, "Could not create window: %s\n",
            SDL_GetError());
      return 1;
   }

   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   int linx = 42, liny = 42;
   face = loadPic("img/test.gif");

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

      SDL_RenderDrawLine(renderer, 2, 2, linx, liny);
      drawPic(face, linx, liny);

      /*
       * Stop drawing things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderPresent(renderer);
   }

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
