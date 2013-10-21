#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int
main()
{
   int x, n, i;
   int result[3] = {0,0,0};

   srand48(time(NULL));
   int ceil=2, floor=0;
   ceil++;

   n = 3;
   for (i = 0; i < 64; i++)
   {
//      while((x = lrand48()) >= RAND_MAX - n && x >= n);
      x = (int) (drand48() * (ceil - floor)) + floor;
      result[x]++;
   }
   puts("jesus rice");

   for (i=0; i<3; i++)
   {
      printf("%d: %d\n", i, result[i]);
   }
   printf("first/second %lf\n", (double)result[0] / result[1]);
   printf("second/third %lf\n", (double)result[1] / result[2]);
   return 0;
}
