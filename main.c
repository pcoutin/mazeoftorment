#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include "clc.h"

int
main(int argc, char *argv[])
{
   CLC_CONFIG config;
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Surface *surface;

   parsecfg("config.cfg", &config);
   //SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

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
   printf("renderer:\t0x%016x\n", renderer);

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
      SDL_RenderDrawLine(renderer, 2, 2, linx, liny);

      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderPresent(renderer);
   }

   sleep(10);

   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}
