#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "../common/mot_maze.h"
#include "server.h"

void *
get_in_addr(struct sockaddr *sa)
{
   if (sa->sa_family == AF_INET)
   {
      return &(((struct sockaddr_in*)sa)->sin_addr);
   }

   return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


size_t
sendall(int s, char *buf, size_t len)
{
   int total = 0;          // how many bytes we've sent
   int bytesleft = len;    // how many we have left to send
   int n;

   while (total < len)
   {
      n = send(s, buf+total, bytesleft, 0);

      if (n == -1)
      {
         perror("sendall");
         return total;
      }

      total += n;
      bytesleft -= n;
   }
   return total;
}


size_t
recvall(int s, char *buf, size_t len)
{
   int total = 0;
   int bytesleft = len;
   int n;

   while (total < len)
   {
      if ((n = recv(s, buf, bytesleft, 0)) == -1)
      {
         perror("recvall");
         return total;
      }
      total += n;
      bytesleft -= n;
   }
   return total;
}


short
getshort(int sock)
{
   short ret;
   recvall(sock, (char *) &ret, sizeof(short));

   return ntohs(ret);
}

/*
 * Return: 0 on success
 *         1 on failure
 */
size_t
sendshort(int sock, short s)
{
   s = htons(s);
   return sendall(sock, (char *) &s, sizeof(s)) != sizeof(s);
}

int
main(int argc, char *argv[])
{
   int ssockfd, csockfd, err;;
   unsigned short magic, x, y;
   unsigned int u;
   struct addrinfo hints, *srvinfo, *p;
   struct sockaddr_storage caddr;
   socklen_t addr_size;
   size_t len, bytes_sent;
   Player_set *pset;

   int i, j;

   /* For handling multiple clients using select() */

   fd_set master;
   fd_set read_fds;  /* File descriptors with readable data */
   int fdmax;        /* Highest file descriptor in the master set */
   int newfd;        /* File descriptor for handling incoming connections */
   char buf[256];
   int nbytes;
   char remoteIP[INET6_ADDRSTRLEN];

   int players_connected = 0;
   time_t launchtime = 0;
   unsigned char game_started = 0;

   /* Should be moved to config file/cli arg ALONG WITH hostname/port */
   int min_players = 2;
   time_t time_thresh = 2;

   FD_ZERO(&master);    /* Empty the master set */
   FD_ZERO(&read_fds);  /* Empty the readfds set */
   pset = calloc(1, sizeof(Player_set));

   genmaze(20, 20);
   memset(&hints, 0, sizeof(hints));

   /* Currently using TCP. */
   hints.ai_family   = AF_UNSPEC;
   hints.ai_flags    = AI_PASSIVE;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   if ((err = getaddrinfo(MOTSRV_ADDR, MOTSRV_PORT, &hints, &srvinfo)))
   {
      fprintf(stderr, "Failed to get address: %s\n", gai_strerror(err));
      return 10;
   }

   /*
    * Allocate socket file descriptor with address information acquired
    * from getaddrinfo.
    */

   if (ssockfd == -1)
   {
      perror("Failed to allocate server socket descriptor.");
      return 7;
   }

   for (p = srvinfo; p != NULL; p = p->ai_next)
   {
      ssockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol);

      if( ssockfd < 0 )
      {
         continue;
      }

      /*
       * If system thinks the socket is on use but it isn't, fix it...
       */
      setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int));

      /*
       * Bind socket to port.
       */
      if (bind(ssockfd, p->ai_addr, p->ai_addrlen) < 0)
      {
         close(ssockfd);
         continue;
      }

      break;
   }


   /*
    * If p reached NULL, it means we didn't bind at all.
    */

   if (p == NULL)
   {
      fprintf( stderr, "failed to bind ssockfd\n" );
      return 2;
   }

   freeaddrinfo(srvinfo);

   /*
    * Start listening on server socket descriptor.
    */

   if (listen(ssockfd, BACKLOG ) == -1)
   {
      perror("Server can't listen.");
      return 3;
   }

   /*
    * Add ssockfd to master set of sockets.
    */
   FD_SET(ssockfd, &master);

   /*
    * Since ssockfd is currently the only socket in master,
    * it's the highest descriptor.
    */
   fdmax = ssockfd;


   while (1)
   {
      read_fds = master;

      if (select(fdmax + 1, &read_fds, NULL, NULL, NULL ) == -1)
      {
         perror("select");
         return 4;
      }

      /*
       * Now read_fds has only those sockets that have readable data to
       * show.
       * Run through all the sockets to find data to read.
       */

      for (i = 0; i <= fdmax; i++)
      {
         if (!FD_ISSET(i, &read_fds))
         {
            continue;
         }

         /* If we're here, i has data to show */
         if (i == ssockfd)
         {
            /* Handle new connections. */

            addr_size = sizeof caddr;
            newfd = accept(ssockfd,
                  (struct sockaddr *) &caddr, &addr_size);

            if (newfd == -1)
            {
               perror("accept");
               continue;
            }

            /* Add the new socket descriptor to master. */
            FD_SET(newfd, &master);

            if (newfd > fdmax) 
            {
               fdmax = newfd;
            }
            printf("server: new connection from %s on socket %d\n",
                  inet_ntop(caddr.ss_family,
                     get_in_addr((struct sockaddr*)&caddr),
                     remoteIP, INET6_ADDRSTRLEN), newfd );

            if (game_started)
            {
               magic = htons(SRV_BUSY);
               sendshort(newfd, magic);
               close(newfd);
               FD_CLR(newfd, &master);
               continue;
            }

            printf("checking if game started!!\n");

            if (!launchtime) launchtime = time(NULL);

            players_connected++;
            handle_connecting_player(newfd, pset);

            printf("new connection handled!!\n");
            if (time(NULL) - launchtime >= time_thresh &&
                  players_connected >= min_players)
            {
               printf("game started!!\n");
               begin_game(pset);
               game_started = 1;
            }
         }
         else
         {
            /* Handle data from a client. (ONLY A SHORT/magic no) */

            if ((nbytes = recv(i, &magic, sizeof magic, 0 )) <= 0)
            {
               if (nbytes < 0)
               {
                  perror("recv");
               }

               printf("server: socket %d hung up\n", i);
               broadcast_disconnect(pset, i, 0);
               close(i);
               FD_CLR(i, &master);

               continue;
            }

            switch (htons(magic))
            {
               case PLAYER_MOV:
                  x = getshort(i);
                  y = getshort(i);
                  printf("player with socket %d moved to %d, %d\n",
                        i, x, y);
                  break;
            }

            for (j = 0; j <= fdmax; j++)
            {
               if (FD_ISSET(j, &master))
               {
                  Player *justMoved = player_byfd(pset, i);
                  Player *coll;
                  int movPnum = justMoved->playerno;

                  justMoved->x = x;
                  justMoved->y = y;

                  /*
                   * If the player that just moved is a hunter, and it
                   * just stepped on a regular player, kill the (regular)
                   * player!
                   */
                  if (justMoved->type &&
                        (coll = check_collision(pset, justMoved)) != NULL)
                  {
                     int dead = coll->fd;

                     printf("%s died, socket %d dropped.\n", coll->name, dead);
                     broadcast_disconnect(pset, dead, 1);
                     close(dead);
                     FD_CLR(dead, &master);
                  }

                  /*
                   * Avoid sending the data to the current client and
                   * server.
                   */
                  if (j != ssockfd
                        && j != i
                        && sendMov(j, movPnum, x, y) == -1)
                  {
                     perror("send");
                  }
               }
            }

         }
      }
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

void
handle_connecting_player(int newfd, Player_set *pset)
{
   char *pname;
   unsigned char pnum;
   unsigned short magic;
   unsigned int u;

   /*
    * When a player first connects, send maze magic, data width, size.
    * Then send the maze itself.  Then await confirmation.
    */

   add_player(pset);
   magic = htons(MAZE_MAGIC);
   sendall(newfd, (char *) &magic, sizeof(magic));

   /* Maze width */
   u = htonl(MAZE.w);
   sendall(newfd, (char *) &u, sizeof(u));

   u = htonl(MAZE.size);
   sendall(newfd, (char *) &u, sizeof(u));

   sendall(newfd, (char *) MAZE.data, MAZE.size);

   if (recv(newfd, &magic, sizeof(magic), 0) != sizeof(magic)
         || ntohs(magic) != MAZE_MAGIC)
   {
      fprintf(stderr, "Failed to get client confirmation\n");
      exit(1);
   }

   pname = malloc(PNAMELEN);
   recvall(newfd, pname, PNAMELEN);
   printf("%s connected !!!\n", pname);

   sendall(newfd, (char *) &pset->last->playerno, sizeof(pnum));

   pset->last->name = pname;
   pset->last->x = -1;
   pset->last->y = -1;
   pset->last->fd = newfd;
   printf("new file descriptor: %d (%d)\n", pset->last->fd,newfd);
}

static void
send_dc(Player *p, int pno_removed, unsigned short sig)
{
   sendshort(p->fd, sig);
   sendshort(p->fd, pno_removed);
}

/*
 * Remove player with file descriptor fd from the list, and broadcast its
 * disconnecting.
 */
void
broadcast_disconnect(Player_set *pset, int fd, int death)
{
   Player *to_remove = player_byfd(pset, fd);
   int remove_pno = to_remove->playerno;
   rm_player(pset, to_remove);
   pset_map(pset, &send_dc, remove_pno, death ? PLAYER_DIE : PLAYER_DC);
}

/*
 * Check if some player in `pset' has the same x, y as `node'. Return the
 * first player that is colliding, or NULL if there are no collisions.
 */
Player *
check_collision(Player_set *pset, Player *node)
{
   Player *temp;

   for (temp = pset->first; temp != NULL; temp = temp->next)
   {
      if (temp != node && temp->x == node->x && temp->y == node->y)
         return temp;
   }
   return NULL;
}


void
set_positions(Player_set *pset)
{
   Player *temp;
   for (temp = pset->first; temp != NULL; temp = temp->next)
   {
      do
      {
         temp->x = mrand(0,19) * 2;
         temp->y = mrand(0,19) * 2;
      } while (check_collision(pset, temp) != NULL);
   }
}


short int
choose_hunter(Player_set *pset)
{
   Player *temp;

   unsigned int hpno = mrand(1, pset->last_pno);

   for (temp = pset->first; temp != NULL; temp = temp->next)
   {
      if (temp->playerno == hpno)
      {
         temp->type = 1;
         return hpno;
      }
   }
   return -1;
}


void
begin_game(Player_set *pset)
{
   unsigned short magic;
   unsigned char hpno;
   int j = 0,i = 0;
   Player *cur, *info;

   set_positions(pset);
   printf("postions set!!\n");

   hpno = choose_hunter(pset);

   for (cur = pset->first; cur != NULL; cur = cur->next)
   {
      for (info = pset->first; info != NULL; info = info->next)
      {
         magic = htons(ADD_PLAYER);
         sendall(cur->fd, (char *) &magic, sizeof(magic));
         sendall(cur->fd, (char *) &info->playerno, sizeof(info->playerno));
         magic = htons(info->x);
         sendall(cur->fd, (char *) &magic, sizeof(magic));
         magic = htons(info->y);
         sendall(cur->fd, (char *) &magic, sizeof(magic));
         sendall(cur->fd, info->name, PNAMELEN);
      }

      // hunter
      magic = htons(HUNTER);
      sendall(cur->fd, (char *) &magic, sizeof(magic));
      sendall(cur->fd, (char *) &hpno, sizeof(hpno));

      printf("hunter at %d sent to fd %d!!!\n", hpno, cur->fd);
   }
   printf("out of begin_game()!!\n");
}


int
sendMov(int psock, short int movepno, int x, int y)
{
   if (sendshort(psock, PLAYER_MOV) ||
         sendshort(psock, movepno)  ||
         sendshort(psock, x)        ||
         sendshort(psock, y))
   {
      return -1;
   }

   return 0;
}
