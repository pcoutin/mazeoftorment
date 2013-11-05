#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "../common/mot_maze.h"

#define MOTSRV_ADDR     "10.0.0.2"
#define MOTSRV_PORT     "6666"
#define BACKLOG         8

#define MAZE_MAGIC     0x6D7A
#define ADD_PLAYER     0x4E45
#define HUNTER         0x4855
#define ILLEGAL_MOV    0xF000
#define PLAYER_MOV     0x4D4F
#define PLAYER_DC      0x4443
#define PLAYER_DIE     0x4B4F
#define PLAYER_WIN     0x5749

int
main(int argc, char *argv[])
{
   int ssockfd, csockfd, err, i;
   unsigned short magic;
   unsigned int u;
   struct addrinfo hints, *srvinfo;
   struct sockaddr_storage caddr;
   socklen_t addr_size;
   size_t len, bytes_sent;

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
    * If system thinks the socket is on use but it isn't, fix it...
    */
   i = 1;
   if (setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) == -1)
   {
      perror("setsockopt");
      return 1;
   }


   /*
    * Bind socket to port.
    */
   if (bind(ssockfd, srvinfo->ai_addr, srvinfo->ai_addrlen) == -1)
   {
      perror("Failed to bind to port");
      return 1;
   }

   if (listen(ssockfd, BACKLOG) == -1)
   {
      perror("Failed to listen to connections");
      return 1;
   }

   /*
    * Accept connections, finally!
    */
   addr_size = sizeof(caddr);
   csockfd = accept(ssockfd, (struct sockaddr *) &caddr, &addr_size);

   if (csockfd == -1)
   {
      perror("Cannot accept connections");
      return 1;
   }

#if 0
   char *msg = "hello world\n";

   len = strlen(msg);
   bytes_sent = send(csockfd, msg, len, 0);
#endif

   magic = htons(MAZE_MAGIC);
   len = sizeof(magic);

   send(csockfd, &magic, len, 0);

   u = htonl(MAZE.w);
   send(csockfd, &u, sizeof(u), 0);

   u = htonl(MAZE.size);
   send(csockfd, &u, sizeof(u), 0);

   send(csockfd, MAZE.data, MAZE.size, 0);

   /*
    * Free things and exit.
    */
   close(ssockfd);
   close(csockfd);
   freeaddrinfo(srvinfo);
   free(MAZE.data);

   return 0;
}
