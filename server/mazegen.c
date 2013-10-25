#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <stddef.h>
#include "../common/mot_maze.h"

#ifdef _DEBUG
static void print_maze();
static void print_maze_old();
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
   if (avoid == 0xf)
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
            if (filledcell_check(MAZE.X, MAZE.Y) != 0xf)
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
   if (avoid == 0xf &&
         MAZE.X == MAZE.width &&
         MAZE.Y == MAZE.height)
   {
      return 0;
   }

   do
   {
      choice = 1 << mrand(0, 3);
      if (avoid == 0xf)
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

/*
 * Checks what edges the cell pointed to by the argument has, in the
 * context of the whole maze (walls and passages).
 * 0: no edges. 255: out of range.
 */
static MCELL
edgecheck(MCELL *c)
{
   MCELL ret = 0;
   size_t curr = (size_t) (c - MAZE.data);

   if (c > MAZE.data + MAZE.size)
   {
      return 255;
   }

   if (curr % MAZE.w == MAZE.w - 1)
   {
      ret |= E_WALL;
   }
   else if (curr % MAZE.w == 0)
   {
      ret |= W_WALL;
   }

   if (curr < MAZE.w)
   {
      ret |= N_WALL;
   }
   else if (curr > MAZE.size - MAZE.w)
   {
      ret |= S_WALL;
   }

   return ret;
}

/*
 * Types of maze tiles:
 *
 * A    B    C      D     E       F      G      H    I      J    K       
 *                                                
 * |        |        |          --|--    |    |---  ---|                    
 * |  ----  ----  ----    |       |    --|--  |        |  |        |       
 * |        |        |  __|__            |                |---  ---|        
 *
 * A: N_WALL | S_WALL
 * B: E_WALL | W_WALL
 * C: N_WALL | S_WALL | E_WALL
 * D: N_WALL | S_WALL | W_WALL
 * E: N_WALL | E_WALL | W_WALL
 * F: S_WALL | E_WALL | W_WALL
 * G: N_WALL | S_WALL | E_WALL | W_WALL
 * H: S_WALL | E_WALL
 * I: S_WALL | W_WALL
 * J: N_WALL | E_WALL
 * K: N_WALL | W_WALL
 *
 * Lone wall: just remove it
 */
static MCELL
setintersect_type(MCELL *curr)
{
   MCELL ret = 0, ec = edgecheck(curr);

   if (!(edgecheck(curr) & N_WALL) && *(curr - MAZE.w))
   {
      ret |= N_WALL;
   }
   if (!(edgecheck(curr) & S_WALL) && *(curr + MAZE.w))
   {
      ret |= S_WALL;
   }
   if (!(edgecheck(curr) & E_WALL) && *(curr + 1))
   {
      ret |= E_WALL;
   }
   if (!(edgecheck(curr) & W_WALL) && *(curr - 1))
   {
      ret |= W_WALL;
   }
   if (*curr)
   {
      *curr = ret;
   }
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
   for (i = 0; i < MAZE.size; i++)
   {
      setintersect_type(MAZE.data + i);
   }

}

#ifdef _DEBUG

int
main(int argc, char *argv[])
{
   FILE *mfile;
   unsigned int w, h;

   if (argc != 3)
   {
      fprintf(stderr, "Usage: %s [width] [height]\n", argv[0]);
      return 1;
   }

   w = atoi(argv[1]);
   h = atoi(argv[2]);

   genmaze(w, h);

   printf("maze is %d bytes long.\n", MAZE.size);

   print_maze_old();
   putchar('\n');
   print_maze();

   mfile = fopen("maze.dat", "w");

   fwrite(&MAZE, sizeof(MAZE), 1, mfile);
   fwrite(MAZE.data, sizeof(MCELL), MAZE.size, mfile);

   fclose(mfile);
   free(MAZE.data);

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
print_maze_old()
{
   size_t i;
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

static void
print_maze()
{
   size_t i;

   freopen(NULL, "w", stdout);

   for (i = 0; i < MAZE.size; i++)
   {
      switch (*(MAZE.data + i))
      {
         case N_WALL:
         case S_WALL:
         case N_WALL | S_WALL:
            printf("│");
            break;
         case E_WALL:
         case W_WALL:
         case E_WALL | W_WALL:
            printf("─");
            break;
         case W_WALL | N_WALL | S_WALL:
            printf("┤");
            break;
         case E_WALL | N_WALL | S_WALL:
            printf("├");
            break;
         case E_WALL | W_WALL | N_WALL:
            printf("┴");
            break;
         case E_WALL | W_WALL | S_WALL:
            printf("┬");
            break;
         case E_WALL | W_WALL | N_WALL | S_WALL:
            printf("┼");
            break;
         case S_WALL | E_WALL:
            printf("┌");
            break;
         case N_WALL | E_WALL:
            printf("└");
            break;
         case N_WALL | W_WALL:
            printf("┘");
            break;
         case S_WALL | W_WALL:
            printf("┐");
            break;
         case 0:
            putchar(' ');
            break;
         default:
            fprintf(stderr, ":(\n");
      }
      if (i % MAZE.w == MAZE.w - 1)
      {
         putchar('\n');
      }
   }
}

#endif
