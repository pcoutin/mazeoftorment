#include <stdio.h>
#include <stdlib.h>

#include "clc.h"

void
drawPic(char *path, int x, int y)
{
   SDL_Surface *pic;
   SDL_Texture *pictex;
   SDL_Rect dstrect;

   pic = IMG_Load(path);
   pictex = SDL_CreateTextureFromSurface(renderer, pic);

   dstrect.x = x;
   dstrect.y = y;
   dstrect.w = pic->w;
   dstrect.h = pic->h;

   SDL_FreeSurface(pic);
   SDL_RenderCopy(renderer, pictex, NULL, &dstrect);
}


int
main(int argc, char *argv[])
{
   CLC_CONFIG config;
   SDL_Window *window;

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

   for (;;)
   {
      SDL_Event e;
      if (SDL_PollEvent(&e)) {
         if (e.type == SDL_QUIT) {
            break;
         }
      }

      SDL_RenderClear(renderer);
      SDL_PumpEvents();

      if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
         SDL_GetMouseState(&linx, &liny);
      }

      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

      /*
       * Draw things.
       */
      SDL_RenderDrawLine(renderer, 2, 2, linx, liny);
      drawPic("img/test.gif", linx, liny);

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderPresent(renderer);
   }

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
