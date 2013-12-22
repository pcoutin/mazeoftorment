/*
 * Client for the Maze of Torment game.
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


void
removep(PLAYER* temp)
{
   if (temp->prev == NULL)
   {
      if (temp->next != NULL)
      {
         temp->next->prev = NULL;
      }
   }
   else
   {
      if (temp->next == NULL)
      {
         temp->prev->next = NULL;
      }
      else
      {
         temp->prev->next = temp->next;
         temp->next->prev = temp->prev;
      }
   }
   free(temp);
}

void
choose_hunter(PLAYER *node, unsigned char hpno, PICTURE *hsprite)
{
   PLAYER *temp;
   for (temp = node; temp != NULL; temp = temp->next)
   {
      if (temp->playerno == hpno)
      {
         temp->type = 1;
         temp->sprite = hsprite;
         break;
      }
   }
}


void
add_player(PLAYER *node, PLAYER *newp)
{
   PLAYER *temp;
   for (temp = node; temp->next != NULL; temp = temp->next);
   temp->next = newp;
   newp->prev = temp;
   newp->next = NULL;
}

unsigned char
addp(PLAYER* node,TCPsocket srv_sock)
{
   Uint16 magic;
   do
   {
      PLAYER cur_player;
      init_player(srv_sock, &cur_player);
      add_player(node,&cur_player);
      printf("Player %s (%d) connected, at (%d, %d)\n", cur_player.name,
            cur_player.playerno, cur_player.x, cur_player.y);
   } while ((magic = getshort(srv_sock)) == ADD_PLAYER);
   
   printf("players added\n");
   
   /*
    * Get the hunter.
    */
   if (magic == HUNTER)
   {
      unsigned char hunter;

      SDLNet_TCP_Recv(srv_sock, &hunter, 1);
      return hunter;
   }
   else
   {
      fprintf(stderr, "Bad magic number %X from server\n", magic);
      exit(EXIT_FAILURE);
   }


}


int
main(int argc, char *argv[])
{
   SDL_Window  *window;
   CLC_CONFIG  config;
   Uint8       *kbdstate;
   SDL_Event   e;
   PLAYER      *me;
   PLAYER      *player;
   Uint32      time;
   IPaddress   srv_ip;
   TCPsocket   srv_sock;
   Uint16      magic;
   int         i;
   SDLNet_SocketSet srv_sset;
   char myname[PNAME_SIZE];
   unsigned char myno;

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

   for (i = 0; i < PNAME_SIZE; i++)
   {
      if (myname[i] == '\n')
      {
         myname[i] = '\0';
         break;
      }
   }


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
    * Add (a single) server socket to srv_sset for cheap hack for checking
    * the server socket's state.
    */
   srv_sset = SDLNet_AllocSocketSet(1);

   if (!srv_sset)
   {
      printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
      exit(EXIT_FAILURE);
   }

   SDLNet_TCP_AddSocket(srv_sset, srv_sock);


   /*
    * Get maze, add connecting players to buffer and wait until the game
    * begins.
    */

   getmaze(srv_sock);

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

   hsprite  = loadPic("img/predator.gif");
   psprite  = loadPic("img/prey.gif");
   black    = loadPic("img/black.gif");

   /*
    * Initialize maze, and send player name.
    */
   MAZE.X = (config.win_width - MAZE.w * 16) / 2;
   MAZE.Y = (config.win_height - MAZE.h * 16) / 2;

   SDLNet_TCP_Send(srv_sock, myname, PNAME_SIZE);


   /*
    * Initialize maze and get the LOCAL player, then the REMOTE players.
    */

   SDLNet_TCP_Recv(srv_sock, &myno, 1);

   player = calloc(MAX_PLAYERNUM + 1, sizeof(PLAYER));

   printf("adding players!!\n");
   
   while ((magic = getshort(srv_sock)) == ADD_PLAYER)
   {
      PLAYER cur_player;
      init_player(srv_sock, &cur_player);
      *(player + cur_player.playerno) = cur_player;
      printf("Player %s (%d) connected, at (%d, %d)\n", cur_player.name,
            cur_player.playerno, cur_player.x, cur_player.y);
      *(player+cur_player.playerno) = cur_player;
   }

   printf("players added\n");
   me = player + myno;

   /*
    * Get the hunter.
    */
   if (magic == HUNTER)
   {
      unsigned char hunter;

      SDLNet_TCP_Recv(srv_sock, &hunter, 1);

      (player + hunter)->type = 1;
      (player + hunter)->sprite = &hsprite;
   }
   else
   {
      fprintf(stderr, "Bad magic number %X from server\n", magic);
      exit(EXIT_FAILURE);
   }

   printf("HUNTER DECIDED\n");
   /*
    * Draw things.
    */
   SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

   /* Draw the maze in the middle of the screen! And draw the players */
   draw_maze(MAZE.X, MAZE.Y);

   drawPlayer(me);

   for (i = 0; i < MAX_PLAYERNUM; ++i)
   {
      if ((player + i) != NULL && (player + i)->sprite != NULL)
      {
         printf("drew player %d\n", i);
         drawPlayer(player + i);
      }
   }

   /*
    * Game loop!
    */
   for (;;)
   {
      time = SDL_GetTicks();

      /*
       * Poll the  network in each frame. Because.
       */

      int numready = SDLNet_CheckSockets(srv_sset, 0);

      if (numready == -1)
      {
         printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
         perror("SDLNet_CheckSockets");
      }
      else if (numready)
      {
         unsigned short packet, hunter;
         int pnum, movx, movy;

         if (SDLNet_TCP_Recv(srv_sock, &packet, 2) == 2)
         {
            switch (SDLNet_Read16(&packet))
            {
               case PLAYER_MOV:
                  puts("PLAYER_MOV");
                  pnum = getshort(srv_sock);
                  movx = getshort(srv_sock);
                  movy = getshort(srv_sock);

                  printf("player %d moved to (%d,%d)\n",
                              pnum, movx, movy);
                  movePlayer(player + pnum, movx, movy);
                  break;
               case PLAYER_WIN:
                  puts("PLAYER_WIN");
                  break;
               case PLAYER_DC:
                  puts("PLAYER_DC");
                  pnum = getshort(srv_sock);
                  clearPlayer(player+pnum);
                  removep(player+pnum);
                  break;
               case ADD_PLAYER:
                  hunter = addp(player,srv_sock);
                  choose_hunter(player,hunter,&hsprite);
                  break;
            }
         }
      }

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

         local_player_update(srv_sock, me, player, SDL_GetKeyboardState(NULL));
      }
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
      /*
       * Stop drawing things.
       */
      SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
      SDL_RenderPresent(renderer);

      if (20 > (SDL_GetTicks() - time))
      {
         SDL_Delay(20 - (SDL_GetTicks() - time));
      }
   }

   SDL_DestroyTexture(psprite.texture);
   SDL_DestroyTexture(hsprite.texture);
   SDL_DestroyTexture(black.texture);
   free(player);
   free(MAZE.data);

   SDL_DestroyWindow(window);

   SDLNet_Quit();
   SDL_Quit();

   return 0;
}
