#include <stdio.h>

typedef struct _linkedlist LINKEDLIST;
typedef struct _ll_elem LLELEM;

struct _linkedlist
{
   LLELEM *elem;  /* really now? */
}

struct _ll_elem
{
   LLELEM *prev;
   LLELEM *next;

   int x, y;
}

void
init_list(LINKEDLIST *list, int nmemb)
{
   LLELEM e, f;

   list->elem = malloc(sizeof(LLELEM));
   list->elem->prev = NULL;

   e = list->elem;

   while (nmemb > 0)
   {
      e->next = malloc(sizeof(LLELEM));
      f = e->next;
      f->prev = e;
      e = f;
      nmemb--;
   }
}
