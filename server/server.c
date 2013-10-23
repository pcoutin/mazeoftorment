#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "mot_maze.h"

#define MOTSRV_ADDR     "10.0.0.1"
#define MOTSRV_PORT     "6666"
#define BACKLOG         8

int
main(int argc, char *argv[])
{
   int ssockfd, csockfd, err;
   struct addrinfo hints, *srvinfo;
   struct sockaddr_storage caddr;
   size_t addr_size;

   genmaze(20, 20);
   memset(&hints, 0, sizeof(hints));

   /* Currently using TCP. */
   hints.ai_family   = AF_UNSPEC;
   hints.ai_flags    = AI_PASSIVE;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   
   if (err = getaddrinfo(MOTSRV_ADDR, MOTSRV_PORT, &hints, &srvinfo))
   {
      fprintf(stderr, "Failed to get address: ", gai_strerror(err));
      return 1;
   }

   /*
    * Allocate socket file descriptor with address information acquired
    * from getaddrinfo.
    */
   ssockfd = socket(
         srvinfo->ai_family,
         srvinfo->ai_socktype,
         srvinfo->ai_protocol
         );

   if (ssockfd == -1)
   {
      perror("Failed to allocate socket descriptor.");
      return 1;
   }

   /*
    * Bind socket to port.
    */
   if (bind(ssockfd, srvinfo->ai_addr, srvinfo->ai_addrlen) == -1)
   {
      perror("Failed to bind to port.");
      return 1;
   }

   if (listen(ssockfd, BACKLOG) == -1)
   {
      perror("Failed to listen to connections.");
      return 1;
   }

   /*
    * Accept connections, finally!
    */
   addr_size = sizeof(caddr);
   csockfd = accept(ssockfd, (struct sockaddr *) &caddr, &addr_size);

   if (csockfd == -1)
   {
      perror("Cannot accept connections.");
      return 1;
   }

   /*
    * wow so useful
    */
   char *msg = "hello world";
   size_t len, bytes_sent;

   len = strlen(msg);
   bytes_sent = send(csockfd, msg, len, 0);

   /*
    * Free things and exit.
    */
   freeaddrinfo(srvinfo);
   free(MAZE.data);

   return 0;
}
