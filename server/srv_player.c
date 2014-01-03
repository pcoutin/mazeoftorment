/*
 * VERY FAST VERY OPTIMIZED BRUTE FORCE LINEAR SEARCH LINKED LIST ALONG
 * WITH select() FOR AMAZING BLAZING FAST DATACENTER GRADE ENTERPRISE
 * SERVER PERFORMANCE
 */

#include <stdlib.h>
#include "server.h"

Player_set *
init_pset()
{
   return calloc(1, sizeof(Player_set));
}

void
free_pset(Player_set *p)
{
   /*
    * Scan through the entire linked list and free each element of each
    * struct, then free p.
    */
   while (p->first != NULL)
   {
      rm_player(p, p->first);
   }

   free(p);
}

/*
 * Add a player at the end of the linked list. Make the current Player
 * element in the set point to this new player.
 */
void
add_player(Player_set *set)
{
   if (set->first == NULL)
   {
      /* Player elements should be initialized to all zeroes. */
      set->first = calloc(1, sizeof(Player));
      set->cur = set->last = set->first;
   }
   else
   {
      set->last->next = calloc(1, sizeof(Player));

      /*
       * Make the prev pointer in the new Player point to the previous
       * player.
       */
      set->last->next->prev = set->last;

      set->cur = set->last = set->last->next;
   }
   set->last->playerno = ++(set->last_pno);
}

void
rm_player(Player_set *set, Player *p)
{
   printf(" XX - Removing player %p\n", p);
   if (p->prev != NULL)
   {
      p->prev->next = p->next;
   }

   if (p->next != NULL)
   {
      p->next->prev = p->prev;
   }

   if (set->first == p)
   {
      set->first = p->next;
   }

   if (set->last == p)
   {
      set->last = p->prev;
   }

   free(p->name);
   free(p);
}

void
print_pset(Player_set *set)
{
   Player *temp;

   for (temp = set->first; temp != NULL; temp = temp->next)
   {
      printf(" * PLAYER %s with pnum %d, fd %d (%p)\n",
            temp->name, temp->playerno, temp->fd, temp);
      printf("   - next: %p\n", temp->next);
      printf("   - prev: %p\n", temp->prev);
   }
}

/*
 * Glorious optimized enterprise-level search algorithm.
 */
Player *
player_byfd(Player_set *s, int fd)
{
   Player *this = s->first;

   while (this != NULL)
   {
      if (this->fd == fd)
      {
         return this;
      }
      this = this->next;
   }
   return NULL;
}

/*
 * blazing fast seeking algorithm
 */
Player *
player_byindex(Player_set *s, int index)
{
   Player *this = s->first;
   int counter = 0;
   while (this != NULL)
   {
      if (counter++ == index)
      {
         return this;
      }
      this = this->next;
   }
   return NULL;
}

/*
 * Check if some player in `pset' has the same x, y as `node'. Return the
 * first player that is colliding, or NULL if there are no collisions.
 */
Player *
check_collision(Player_set *pset, Player *node)
{
   Player *temp;

   for (temp = pset->first; temp != NULL; temp = temp->next)
   {
      if (temp != node && temp->x == node->x && temp->y == node->y)
         return temp;
   }
   return NULL;
}

void
pset_map(Player_set *s,
         void (*func)(Player *p, int a, unsigned short sig),
         int fd,
         unsigned short sig)
{
   Player *this = s->first;

   while (this != NULL)
   {
      func(this, fd, sig);
      this = this->next;
   }
}
