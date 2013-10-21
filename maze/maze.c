#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAZE_WIDTH      20
#define MAZE_HEIGHT     20

#define N_WALL          0b0001
#define E_WALL          0b0010
#define S_WALL          0b0100
#define W_WALL          0b1000

typedef unsigned char   MCELL;

void print_maze();
void print_code(unsigned char dircode);

struct
{
   MCELL *data;

   /*
    * w and h are the "data w": not the number of cells in a row or
    * column, but the number of cells AND walls in a row or column.
    */
   unsigned int w, h;

   /*
    * The killer's/drunkard's/maze carver's current X and Y position.
    */
   unsigned int X, Y;

   /*
    * Data size of the maze representation, in bytes.
    */
   size_t size;
} MAZE;

MCELL *
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
unsigned short
edge_check(unsigned int x, unsigned int y)
{
   unsigned short ret = 0;
   if (x == 0)
   {
      ret |= W_WALL;
   }
   else if (x == MAZE_WIDTH - 1)
   {
      ret |= E_WALL;
   }
   
   if (y == 0)
   {
      ret |= N_WALL;
   }
   else if (y == MAZE_HEIGHT - 1)
   {
      ret |= S_WALL;
   }

   return ret;
}

unsigned short
wall_check(unsigned int x, unsigned int y)
{
   unsigned short ret = 0;

   /*
    * A mazecell() return value will never be on a wall.
    * Thus, there's no need to worry about whether we're on an edge and
    * there is nothing to the right or left, or on the top or bottom.
    */
   if (*(mazecell(x, y) + 1))
   {
      ret |= E_WALL;
   }
   if (*(mazecell(x, y) - 1))
   {
      ret |= W_WALL;
   }

   if (*(mazecell(x, y) - MAZE.w))
   {
      ret |= N_WALL;
   }
   if (*(mazecell(x, y) + MAZE.w))
   {
      ret |= S_WALL;
   }

   return ret;
}

/*
 * Check for directions that should be avoided, which are outside the
 * grid, and to a visited cell.
 */
unsigned short
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

int
mrand(int floor, int ceil)
{
   return (int) (drand48() * (ceil + 1 - floor)) + floor;
}

/*
 * Randomly choose to go to a cell left/right/up/down.
 * It must have been unvisited. If stuck, hunt.
 * Also, don't jump off the edge.
 */
void
step()
{
   /*
    * Yes.
    */
   unsigned char avoid = avoid_check(MAZE.X, MAZE.Y);
   unsigned char choice;

   /*
    * If we're stuck, hunt.
    */
   if (avoid == 0b1111)
   {
      puts("\nWe're stuck!");
      print_maze();
      exit(0);
   }

   do
   {
      choice = 1 << mrand(0, 3);
   } while(choice & avoid);

   printf("avoid, choice:\t");
   print_code(avoid);
   putchar('\t');
   putchar('\t');
   print_code(choice);
   putchar('\n');


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
         break;
      default:
         fprintf(stderr, "wtf");
   }
}

void
print_maze()
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

int
main()
{
   MAZE.w =       MAZE_WIDTH * 2 + 1;
   MAZE.h =       MAZE_HEIGHT * 2 + 1;
   MAZE.size =    MAZE.w * MAZE.h * sizeof(MCELL);
   MAZE.data =    (MCELL *) malloc(MAZE.size);

   memset(MAZE.data, 1, MAZE.size);
   srand48(time(NULL));

   unsigned int x, y;

   printf("maze is %d bytes long.\n", MAZE.size);

   /*
    * Start in a random cell, okay?
    */
   MAZE.X = mrand(0, MAZE_WIDTH - 1);
   MAZE.Y = mrand(0, MAZE_HEIGHT - 1);
   printf("wow my cell is at like %d, %d\n", MAZE.X, MAZE.Y);
   *mazecell(MAZE.X, MAZE.Y) = 0;
   print_maze();
   for(;;) { step(); print_maze(); }

   return 0;
}

void
print_code(unsigned char dc)
{
   if (dc & N_WALL) putchar('N');
   if (dc & S_WALL) putchar('S');
   if (dc & W_WALL) putchar('W');
   if (dc & E_WALL) putchar('E');
   if (!dc) printf("NONE");
}
