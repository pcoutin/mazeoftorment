#include <sys/types.h>
#include <socket.h>
#include "mot_maze.h"

int
main(int argc, char *argv[])
{
   struct addrinfo hints, *res;
   int sock;

   genmaze();

   memset(&hints, 0, sizeof(hints));

   /*
    * SOCK_STREAM is basically TCP... or implemented on top of TCP,
    * rather. SOCK_DGRAM would be more like UDP.
    */
   hints.ai_family   = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags    = AI_PASSIVE;

   return 0;
}
