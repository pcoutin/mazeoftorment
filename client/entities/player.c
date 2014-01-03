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

/*
 * Move the player `p' TO a specified x, y location (assigns, doesn't
 * translate).
 */
void
movePlayer(PLAYER *p, short newx, short newy)
{
   clearPlayer(p);
   p->x = newx;
   p->y = newy;
   clearPlayer(p);
   drawPlayer(p);
}

void
drawPlayer(PLAYER *player)
{
   printf("inside drawplayer()!!\n");
   if (player->dead)
   {
      return;
   }
   player->sprite->rect.x = MAZE.X + player->x * TILE_WIDTH + 10;
   player->sprite->rect.y = MAZE.Y + player->y * TILE_HEIGHT + 10;
   printf("changed player sprite!!\n");
   SDL_RenderCopy(renderer,
         player->sprite->texture,
         NULL, &(player->sprite->rect));
   printf("exiting drawplayer()!!\n");
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

/*
 * Return the player with pnode `pnum' from the player linked list
 * starting with `node'.
 */
PLAYER *
choose_player(PLAYER *node, unsigned char pnum)
{
   PLAYER *temp;
   for (temp = node->next; temp != NULL; temp = temp->next)
   {  
      printf("temp->playerno = %d, pnum = %d\n", temp->playerno, pnum);
      if (temp->playerno == pnum)
      {
         break;
      }
   }
   return temp;
}

/*
 * Free the player `temp' after linking the two players next to `temp'
 * together. Also erases it from the screen.
 */
//potential bug: hunter eats someone, then someone disconnects. they both
//have the same location, so they both disappear?
void
removep(PLAYER *temp)
{
   if (temp == NULL)
   {
      printf("Can't remove NULL player!!\n");
      return;
   }
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
   clearPlayer(temp);
   free(temp);
}

/*
 * Blindly iterate through the player linked list starting from `node' to
 * find the player with pnode `hpno', and set it to be of hunter type.
 */
void
choose_hunter(PLAYER *node, unsigned char hpno)
{
   PLAYER *temp;

   for (temp = node; temp != NULL; temp = temp->next)
   {
      if (temp->playerno == hpno)
      {
         temp->type = 1;
         temp->sprite = &hsprite;
         return;
      }
   }
}

/*
 * Add an initialized player "object" to the player linked list containing
 * `node'.
 */
void
add_player(PLAYER *node, PLAYER *newp)
{
   PLAYER *temp;

   for (temp = node; temp->next != NULL; temp = temp->next)
   {
      printf("cycle: temp->next->playerno: %d\n",temp->next->playerno);
   }

   for (temp = node; temp->next != NULL; temp = temp->next);

   temp->next = newp;
   newp->prev = temp;
   newp->next = NULL;
}

/*
 * Keep receiving players from the server until it's done and it chooses a
 * hunter, adding them to the player linked list containing `node'.
 */
unsigned char
addp(PLAYER *node, TCPsocket srv_sock)
{
   Uint16 magic;
   PLAYER *cur_player = NULL;

   do
   {
      cur_player = calloc(1, sizeof(PLAYER));
      init_player(srv_sock, cur_player);
      add_player(node, cur_player);

      printf("Player %s (%d) connected, at (%d, %d)\n", cur_player->name,
            cur_player->playerno, cur_player->x, cur_player->y);

   } while ((magic = getshort(srv_sock)) == ADD_PLAYER);
   
   printf("players added\n");
   
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
