#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "../common/mot_maze.h"

#define MOTSRV_ADDR     "127.0.0.1"
#define MOTSRV_PORT     "6666"
#define BACKLOG         8

#define MAZE_MAGIC      0x6D7A
#define ADD_PLAYER      0x4E45
#define HUNTER          0x4855
#define ILLEGAL_MOV     0xF000
#define PLAYER_MOV      0x4D4F
#define PLAYER_DC       0x4443
#define PLAYER_DIE      0x4B4F
#define PLAYER_WIN      0x5749
#define SRV_BUSY        0xEEEE

int mrand(int floor, int ceil);

/*
 * Thanks to Brian Hall for Beej's Guide to Network Programming
 * This function is taken from the guide, and this function is under
 * public domain. This function was modified.
 */
void
sendall(int s, char *buf, size_t len)
{
   int total = 0;          // how many bytes we've sent
   int bytesleft = len;    // how many we have left to send
   int n;

   while(total < len)
   {
      n = send(s, buf+total, bytesleft, 0);

      if (n == -1)
      {
         perror("sendall");
         exit(EXIT_FAILURE);
      }

      total += n;
      bytesleft -= n;
   }
} 

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

   /*
    * When a player first connects, send maze magic, data width, size.
    * Then send the maze itself. Then await confirmation.
    */
   magic = htons(MAZE_MAGIC);
   sendall(csockfd, (char *) &magic, sizeof(magic));

   u = htonl(MAZE.w);
   sendall(csockfd, (char *) &u, sizeof(u));

   u = htonl(MAZE.size);
   sendall(csockfd, (char *) &u, sizeof(u));

   sendall(csockfd, MAZE.data, MAZE.size);

   if (recv(csockfd, &magic, sizeof(magic), 0) != sizeof(magic) ||
         ntohs(magic) != MAZE_MAGIC)
   {
      fprintf(stderr, "Failed to get client confirmation\n");
      exit(EXIT_FAILURE);
   }

   /*
    * Receive player name (32 byte string), then send player number.
    */
   char PLAYANAME[32];
   recv(csockfd, &PLAYANAME, 32, 0);
   printf("%s connected!!!\n", PLAYANAME);

   unsigned char pnum = 0;
   sendall(csockfd, &pnum, sizeof(pnum));

   /*
    * Add a few players scattered across the maze, then pick one as the
    * predator...
    */
   for (pnum = 0; pnum < 12; pnum++)
   {
      /* player no */
      magic = htons(ADD_PLAYER);
      sendall(csockfd, (char *) &magic, sizeof(magic));
      sendall(csockfd, &pnum, sizeof(pnum));

      /* x and y, there can be collisions but who cares?????? */
      magic = htons(mrand(0, 19) * 2);
      sendall(csockfd, (char *) &magic, sizeof(magic));

      magic = htons(mrand(0, 19) * 2);
      sendall(csockfd, (char *) &magic, sizeof(magic));

      sendall(csockfd, PLAYANAME, 32);
   }

   /*
    * Free things (sockets, addrinfo, player data, maze) and exit.
    */
   close(ssockfd);
   close(csockfd);
   freeaddrinfo(srvinfo);
   free(MAZE.data);

   return 0;
}
