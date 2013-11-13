#include <SDL2/SDL.h>
#include "../../common/mot_maze.h"
#include "entities.h"
#include "../mot.h"

/*
 * initx and inity are in terms of the maze cells, not pixels.
 */
void
init_localplayer(PLAYER *player,
      short initx,
      short inity,
      unsigned char ishunter,
      unsigned char playerno,
      PICTURE *sprite,
      char *pname)
{
   player->x = initx;
   player->y = inity;
   player->type = ishunter;
   player->sprite = sprite;
   player->dead = 0;
   player->playerno = playerno;
   player->name = pname;
}

/*
 * Checks array of PLAYER structs to see if one contains the location
 * given...
 * O(n)
 */
PLAYER *
playerAt(PLAYER *players, short x, short y)
{
   int i;
   for (i = 0; players[i].sprite != NULL; i++)
   {
      if (players[i].x == x && players[i].y == y)
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
local_player_update(PLAYER *me, PLAYER *remote, const Uint8 *kbdstate)
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
   if ((dunce = playerAt(remote, me->x, me->y)) != NULL)
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
