#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <SDL2/SDL_net.h>

#include "mot.h"
#include "net.h"

int
main(int argc, char *argv[])
{
   SDL_Window  *window;
   PICTURE     predator;
   PICTURE     prey;
   CLC_CONFIG  config;
   Uint8       *kbdstate;
   SDL_Event   e;
   PLAYER      me;
   PLAYER      *remote;
   Uint32 time;
   IPaddress   srv_ip;
   TCPsocket   srv_sock;
   int i;

   /* debug, should remove later */
   FILE        *mfile;

   if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1)
   {
      fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
   }

   if (SDLNet_Init() == -1)
   {
      fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
      exit(EXIT_FAILURE);
   }

   parsecfg(&config);

   /*
    * Connect to server!
    */
   if (SDLNet_ResolveHost(&srv_ip, config.defaultsrv,
            atoi(config.defaultport)))
   {
      fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
      exit(EXIT_FAILURE);
   }

   /*
    * Bind socket!
    */
   if (!(srv_sock = SDLNet_TCP_Open(&srv_ip)))
   {
      fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
      exit(EXIT_FAILURE);
   }

   /*
    * Get maze, add connecting players to buffer and wait until the game
    * begins.
    */

   if (getmaze(srv_sock))
   {
      exit(EXIT_FAILURE);
   }

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
      exit(EXIT_FAILURE);
   }

   renderer = SDL_CreateRenderer(window, -1, config.renderflags);

   predator = loadPic("img/predator.gif");
   prey     = loadPic("img/prey.gif");
   black    = loadPic("img/black.gif");

#if 0
   /*
    * debug load maze from a file
    */

   mfile = fopen("maze.dat", "r");
   fread(&MAZE, sizeof(MAZE), 1, mfile);
   MAZE.data = malloc(MAZE.size * sizeof(MCELL));
   fread(MAZE.data, sizeof(MCELL), MAZE.size, mfile);
   fclose(mfile);
#endif

   /*
    * Initialize maze.
    */
   MAZE.X = (config.win_width - MAZE.w * 16) / 2;
   MAZE.Y = (config.win_height - MAZE.h * 16) / 2;

   /* Type is 1 (hunter)... FOR NOW */
   init_player(&me, 6, 6, 1, 0, &predator);

   /*
    * Draw things.
    */
   SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

   /* Draw the maze in the middle of the screen! And draw the players */
   draw_maze(MAZE.X, MAZE.Y);

   drawPlayer(&me);

   remote = calloc(4, sizeof(PLAYER));

   init_player(remote, 0, 0, 0, 1, &prey);
   init_player(remote + 1, 2, 0, 0, 2, &prey);

   for (i = 0; (remote + i)->sprite != NULL; i++)
   {
      drawPlayer(remote + i);
   }

   /*
    * Game loop!
    */
   for (;;)
   {
      time = SDL_GetTicks();

      /*
       * Poll the network
       */

      /*
       * Poll for keys
       */
      if (SDL_PollEvent(&e))
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

         local_player_update(&me, remote, SDL_GetKeyboardState(NULL));
      }

      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

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

   SDL_DestroyTexture(predator.texture);
   SDL_DestroyTexture(prey.texture);
   free(remote);
   free(MAZE.data);

   SDL_DestroyWindow(window);

   SDLNet_Quit();
   SDL_Quit();

   return 0;
}
