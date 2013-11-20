#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "../../common/mot_maze.h"
#include "entities.h"
#include "../mot.h"
#include "../net.h"

void
init_player(TCPsocket sock, PLAYER *player)
{
   short loc;
   char *pname;

   SDLNet_TCP_Recv(sock, &player->playerno, sizeof(unsigned char));

   SDLNet_TCP_Recv(sock, &loc, sizeof(loc));
   player->x = SDLNet_Read16(&loc);

   SDLNet_TCP_Recv(sock, &loc, sizeof(loc));
   player->y = SDLNet_Read16(&loc);

   pname = malloc(PNAME_SIZE);
   SDLNet_TCP_Recv(sock, pname, PNAME_SIZE);

   player->name = pname;

   /* default type is 0 */
   player->type = 0;

#ifdef _DEBUG
   printf("Got %s (%d)", pname, player->playerno);
#endif

   player->sprite = &psprite;
   player->dead = 0;
}

/*
 * Checks array of PLAYER structs to see if one contains the location
 * given...
 * O(n)
 */
PLAYER *
playerAt(PLAYER *players, short x, short y, PLAYER *me)
{
   int i;
   for (i = 0; players[i].sprite != NULL; i++)
   {
      if (players[i].x == x && players[i].y == y && players + i != me)
      {
         return &players[i];
      }
   }
   return NULL;
}

/*
 * Update the user of this client (not the remote players).
 * reimplement later by asking the server if we can move and then moving?
 * also use higher level functions to move by one cell somewhere
 * i mean what
 */
void
local_player_update(TCPsocket sock, PLAYER *me, PLAYER *remote,
      const Uint8 *kbdstate)
{
   short myx = me->x,
         myy = me->y;

   /* Update based on walls... */
   if (kbdstate[SDL_SCANCODE_DOWN] && !mazetile(me->x + 1, me->y + 2))
   {
      clearPlayer(me);
      me->y += 2;
   }
   else if (kbdstate[SDL_SCANCODE_UP] && !mazetile(me->x + 1, me->y))
   {
      clearPlayer(me);
      me->y -= 2;
   }
   else if (kbdstate[SDL_SCANCODE_RIGHT] && !mazetile(me->x + 2, me->y + 1))
   {
      clearPlayer(me);
      me->x += 2;
   }
   else if (kbdstate[SDL_SCANCODE_LEFT] && !mazetile(me->x, me->y + 1))
   {
      clearPlayer(me);
      me->x -= 2;
   }
   else
   {
      return;
   }

   PLAYER *dunce;

   if ((dunce = playerAt(remote, me->x, me->y, me)) != NULL)
   {
      /* Not a hunter */
      if (!me->type)
      {
         /* Reset/don't move */
         me->x = myx;
         me->y = myy;
      } /* Hunter */
      else
      {
         /* Kill the other player and take its spot :( */
         clearPlayer(dunce);
         dunce->dead = 1;
         dunce->x = -5;
         dunce->y = -5;
      }
   }

   sendshort(sock, PLAYER_MOV);
   sendshort(sock, me->x);
   sendshort(sock, me->y);

   drawPlayer(me);
}

void
drawPlayer(PLAYER *player)
{
   if (player->dead)
   {
      return;
   }
   player->sprite->rect.x = MAZE.X + player->x * TILE_WIDTH + 10;
   player->sprite->rect.y = MAZE.Y + player->y * TILE_HEIGHT + 10;
   SDL_RenderCopy(renderer,
         player->sprite->texture,
         NULL, &player->sprite->rect);
}

/*
 * Reset the maze cell that the player is on...
 */
void
clearPlayer(PLAYER *player)
{
   player->sprite->rect.x = MAZE.X + player->x * TILE_WIDTH + 10;
   player->sprite->rect.y = MAZE.Y + player->y * TILE_HEIGHT + 10;
   SDL_RenderCopy(renderer,
         black.texture,
         NULL, &player->sprite->rect);
}
