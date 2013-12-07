#ifndef _MOT_MAZE_H
#define _MOT_MAZE_H

#define N_WALL          0x1
#define E_WALL          0x2
#define S_WALL          0x4
#define W_WALL          0x8

struct
{
   unsigned char *data;

   /*
    * w and h are the "data w": not the number of cells in a row or
    * column, but the number of cells AND walls in a row or column.
    */
   size_t w, h;

   /*
    * Cell width and height of the maze, not counting walls. (Width and
    * height of the maze in pixels for the client)
    */
   unsigned int width, height;

   /*
    * The killer's/drunkard's/maze carver's current X and Y position in
    * the generator. The maze's top left corner in the client. (in pixels)
    */
   unsigned int X, Y;

   /*
    * Data size of the maze representation, in bytes.
    */
   size_t size;
} MAZE;

#define MTILE(x, y) *(MAZE.data + x + y * MAZE.h)

void genmaze(unsigned int width, unsigned int height);

#endif
