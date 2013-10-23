#ifndef _MOT_MAZE_H
#define _MOT_MAZE_H

typedef unsigned char      MCELL;

struct
{
   MCELL *data;

   /*
    * w and h are the "data w": not the number of cells in a row or
    * column, but the number of cells AND walls in a row or column.
    */
   size_t w, h;

   /*
    * Cell width and height of the maze, not counting walls.
    */
   unsigned int width, height;

   /*
    * The killer's/drunkard's/maze carver's current X and Y position.
    * (Used for generating)
    */
   unsigned int X, Y;

   /*
    * Data size of the maze representation, in bytes.
    */
   size_t size;
} MAZE;

void genmaze(unsigned int width, unsigned int height);

#endif
