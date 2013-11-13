/*
 * Simple client for the Maze of Torment game.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#include "../common/mot_maze.h"
#include "entities/entities.h"
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
   char myname[PNAME_SIZE];
   unsigned char myno;
   int i;

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
    * Get player name.
    */
   printf("Wow such name: ");
   fgets(myname, PNAME_SIZE, stdin);

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

   getmaze(srv_sock, myname, &myno);

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

   /*
    * Initialize maze.
    */
   MAZE.X = (config.win_width - MAZE.w * 16) / 2;
   MAZE.Y = (config.win_height - MAZE.h * 16) / 2;

   /* Type is 1 (hunter)... FOR NOW */
   init_localplayer(&me, 6, 6, 1, myno, &predator, myname);

   /*
    * Draw things.
    */
   SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

   /* Draw the maze in the middle of the screen! And draw the players */
   draw_maze(MAZE.X, MAZE.Y);

   drawPlayer(&me);

   remote = calloc(MAX_PLAYERNUM, sizeof(PLAYER));

   while (getshort(srv_sock) == ADD_PLAYER)
   {
      PLAYER cur_remote;
      init_player(srv_sock, &cur_remote, &prey);
      *(remote + cur_remote.playerno) = cur_remote;
      printf("Added player %d at (%d, %d)\n", cur_remote.playerno,
            cur_remote.x, cur_remote.y);
   }

   for (i = 0; (remote + i)->sprite != NULL; i++)
   {
      drawPlayer(remote + i);
      puts("draw player");
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
