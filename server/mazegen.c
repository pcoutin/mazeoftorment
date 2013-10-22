#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mot_server.h"

#define N_WALL          0b0001
#define E_WALL          0b0010
#define S_WALL          0b0100
#define W_WALL          0b1000

#ifdef _DEBUG
static void print_maze();
static void print_code(unsigned char dircode);
#endif

static MCELL *
mazecell(unsigned int x, unsigned int y)
{
   /*
    * The first cell will actually be at MAZE.w + 1.
    * The stuff above is a wall.
    */
   MCELL *ret = MAZE.data + (MAZE.w + 1) + (2 * x) + (MAZE.w * 2 * y);

   if (ret - MAZE.data > MAZE.size)
   {
      return NULL;
   }
   return ret;
}

/*
 * Right-to-left:
 * bit 0:    N_WALL
 * bit 1:    E_WALL
 * bit 2:    S_WALL
 * bit 3:    W_WALL
 */
static unsigned short
edge_check(unsigned int x, unsigned int y)
{
   unsigned short ret = 0;
   if (x == 0)
   {
      ret |= W_WALL;
   }
   else if (x == MAZE.width - 1)
   {
      ret |= E_WALL;
   }
   
   if (y == 0)
   {
      ret |= N_WALL;
   }
   else if (y == MAZE.height - 1)
   {
      ret |= S_WALL;
   }

   return ret;
}

/*
 * Check for directions that should be avoided, which are outside the
 * grid, and to a visited cell.
 *
 * Bits for directions in which there's an edge of the maze or an empty
 * cell will be set.
 */
static unsigned char
avoid_check(unsigned int x, unsigned int y)
{
   unsigned char ret = edge_check(x, y);

   if (!(ret & N_WALL) && !*mazecell(x, y - 1))
   {
      ret |= N_WALL;
   }
   if (!(ret & S_WALL) && !*mazecell(x, y + 1))
   {
      ret |= S_WALL;
   }
   if (!(ret & W_WALL) && !*mazecell(x - 1, y))
   {
      ret |= W_WALL;
   }
   if (!(ret & E_WALL) && !*mazecell(x + 1, y))
   {
      ret |= E_WALL;
   }
   return ret;
}

/*
 * Bits for directions in which there's an edge of the maze or a filled
 * cell will be set.
 *
 * Direction bits set to zero are empty cells.
 */
static unsigned char
filledcell_check(unsigned int x, unsigned int y)
{
   unsigned char ret = edge_check(x, y);

   if (!(ret & N_WALL) && *mazecell(x, y - 1))
   {
      ret |= N_WALL;
   }
   if (!(ret & S_WALL) && *mazecell(x, y + 1))
   {
      ret |= S_WALL;
   }
   if (!(ret & W_WALL) && *mazecell(x - 1, y))
   {
      ret |= W_WALL;
   }
   if (!(ret & E_WALL) && *mazecell(x + 1, y))
   {
      ret |= E_WALL;
   }
   return ret;
}

static int
mrand(int floor, int ceil)
{
   return (int) (drand48() * (ceil + 1 - floor)) + floor;
}

/*
 * Randomly choose to go to a cell left/right/up/down.
 * It must have been unvisited. If stuck, hunt.
 * Also, don't jump off the edge.
 */
static int
step()
{
   unsigned char choice, avoid = avoid_check(MAZE.X, MAZE.Y);

   /*
    * If we're stuck, hunt.
    */
   if (avoid == 0b1111)
   {
      for (MAZE.X = 0; MAZE.X < MAZE.width; MAZE.X++)
      {
         for (MAZE.Y = 0; MAZE.Y < MAZE.height; MAZE.Y++)
         {
            /*
             * If the current cell is visited, keep searching.
             */
            if (!*mazecell(MAZE.X, MAZE.Y))
            {
               continue;
            }
            /*
             * Break if the current cell hasn't been visited (already
             * checked) but it's adjacent to a visited cell.
             */
            if (filledcell_check(MAZE.X, MAZE.Y) != 0b1111)
            {
               goto _considered_harmful;
            }
         }
      }
   }

   /*
    * Move anywhere but to an edge. Try to avoid moving to already visited
    * cells, but it's possible to move to visited cells???
    *
    * Found out through gdb: Somehow, if the loop ends without breaking,
    * MAZE.X and MAZE.Y are set to the width and height, respectively. But
    * why?
    */

   _considered_harmful:
   if (avoid == 0b1111 &&
         MAZE.X == MAZE.width &&
         MAZE.Y == MAZE.height)
   {
      return 0;
   }

   do
   {
      choice = 1 << mrand(0, 3);
      if (avoid == 0b1111)
      {
         /*
          * Just pick a random empty cell.
          */
         if (~filledcell_check(MAZE.X, MAZE.Y) & choice)
         {
            break;
         }
      }
   } while(choice & avoid);

#ifdef _DEBUG
/*   printf("avoid, choice:\t");
   print_code(avoid);
   putchar('\t');
   putchar('\t');
   print_code(choice);
   putchar('\n'); */
#endif

   switch (choice)
   {
      case N_WALL:
         *(mazecell(MAZE.X, MAZE.Y) - MAZE.w) = 0;
         *mazecell(MAZE.X, --MAZE.Y) = 0;
         break;
      case S_WALL:
         *(mazecell(MAZE.X, MAZE.Y) + MAZE.w) = 0;
         *mazecell(MAZE.X, ++MAZE.Y) = 0;
         break;
      case W_WALL:
         *(mazecell(MAZE.X, MAZE.Y) - 1) = 0;
         *mazecell(--MAZE.X, MAZE.Y) = 0;
         break;
      case E_WALL:
         *(mazecell(MAZE.X, MAZE.Y) + 1) = 0;
         *mazecell(++MAZE.X, MAZE.Y) = 0;
   }
   return 1;
}

void
genmaze(unsigned int width, unsigned int height)
{
   unsigned int i;

   MAZE.width =   width;
   MAZE.height =  height;
   MAZE.w =       MAZE.width * 2 + 1;
   MAZE.h =       MAZE.height * 2 + 1;
   MAZE.size =    MAZE.w * MAZE.h * sizeof(MCELL);
   MAZE.data =    (MCELL *) malloc(MAZE.size);

   memset(MAZE.data, 1, MAZE.size);
   srand48(time(NULL));

   /*
    * Start in a random cell.
    */
   MAZE.X = mrand(0, MAZE.width - 1);
   MAZE.Y = mrand(0, MAZE.height - 1);

   *mazecell(MAZE.X, MAZE.Y) = 0;

   /*
    * Carve maze.
    */
   while (step());

   /*
    * Find a spot to put an exit on. First, choose the edge: north, south,
    * east or west. Then, find an empty cell along the edge. Then, carve
    * out the edge adjacent to the cell.
    */
   switch (1 << mrand(0, 3))
   {
      case N_WALL:

         puts("North wall");
         for (
            MAZE.X = mrand(0, MAZE.width - 2);
            *mazecell(MAZE.X, 0);
            ++MAZE.X
         );

         *(mazecell(MAZE.X, 0) - MAZE.w) = 0;
         break;

      case S_WALL:

         for (
            MAZE.X = mrand(0, MAZE.width - 2);
            *mazecell(MAZE.X, MAZE.height - 1);
            ++MAZE.X
         );

         *(mazecell(MAZE.X, MAZE.height - 1) + MAZE.w) = 0;
         break;

      case E_WALL:

         for (
            MAZE.Y = mrand(0, MAZE.height - 2);
            *mazecell(MAZE.width - 1, MAZE.Y);
            ++MAZE.Y
         );

         *(mazecell(MAZE.width - 1, MAZE.Y) + 1) = 0;
         break;

      case W_WALL:

         for (
            MAZE.Y = mrand(0, MAZE.height - 2);
            *mazecell(0, MAZE.Y);
            ++MAZE.Y
         );

         *(mazecell(0, MAZE.Y) - 1) = 0;
         break;
   }

   /*
    * Find kinds of tree intersections
    */
}

#ifdef _DEBUG

int
main()
{
   genmaze(20, 20);

   printf("maze is %d bytes long.\n", MAZE.size);

   print_maze();

   return 0;
}

static void
print_code(unsigned char dc)
{
   if (dc & N_WALL) putchar('N');
   if (dc & S_WALL) putchar('S');
   if (dc & W_WALL) putchar('W');
   if (dc & E_WALL) putchar('E');
   if (!dc) printf("NONE");
}

static void
print_maze()
{
   size_t i;
   *(MAZE.data + 1621) = 123;
   for (i = 0; i < MAZE.size; i++)
   {
      if (*(MAZE.data + i))
      {
         putchar('#');
      }
      else
      {
         putchar(' ');
      }
      putchar(' ');

      if (i % MAZE.w == MAZE.w - 1)
      {
         putchar('\n');
      }
   }
}
#endif
