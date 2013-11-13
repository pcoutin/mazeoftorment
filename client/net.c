#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include "../common/mot_maze.h"
#include "entities/entities.h"
#include "mot.h"
#include "net.h"

int
killPlayer(unsigned short playerno)
{
   return 0;
}

size_t
recvall(TCPsocket sock, unsigned char *data, size_t len)
{
   size_t n, brecv = 0;
   size_t bytesleft = len;

   while (brecv < len)
   {
      n = SDLNet_TCP_Recv(sock, data + brecv, bytesleft);
      printf("got %ld\n", n);

      if (n == -1)
      {
         fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
         return -1;
      }
      brecv += n;
      bytesleft -= n;
   }
   return brecv;
}


Uint16
getshort(TCPsocket sock)
{
   Uint16 cmd;
   Uint16 recvdata = SDLNet_TCP_Recv(sock, &cmd, sizeof(cmd));

   if (recvdata != sizeof(cmd))
   {
      return 0;
   }
   return SDLNet_Read16(&cmd);
}

void
sendshort(TCPsocket sock, Uint16 i)
{
   Uint16 cmd;
   int bytes_sent;
   SDLNet_Write16(i, &cmd);
   if ((bytes_sent = SDLNet_TCP_Send(sock, &cmd, sizeof(cmd))) != sizeof(cmd))
   {
      fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
   }
}

Uint32
getint(TCPsocket sock)
{
   Uint32 cmd;
   int recvdata = SDLNet_TCP_Recv(sock, &cmd, sizeof(cmd));

   if (recvdata != sizeof(cmd))
   {
      return 0;
   }
   return SDLNet_Read32(&cmd);
}

void
getmaze(TCPsocket sock, char *pname, unsigned char *pno)
{
   Uint16 n;
   int recv;

   switch (n = getshort(sock))
   {
      case MAZE_MAGIC:
         break;
      case SRV_BUSY:
         fprintf(stderr, "Game is already going on :(\n");
         exit(EXIT_FAILURE);
      default:
         fprintf(stderr, "Bad magic number from server: %x\n", n);
         exit(EXIT_FAILURE);
   }

   MAZE.w = getint(sock);
   MAZE.size = getint(sock);
   MAZE.h = MAZE.size / MAZE.w;

   printf("w size h = %ld %ld %ld\n", MAZE.w, MAZE.size, MAZE.h);

   MAZE.data = malloc(MAZE.size);

   // uh no
   if ((recv = recvall(sock, MAZE.data, MAZE.size)) != MAZE.size)
   {
      fprintf(stderr, "Failed to get maze. Got %d bytes, expected %ld. %s\n",
            recv, MAZE.size, SDLNet_GetError());
      exit(EXIT_FAILURE);
   }

   sendshort(sock, MAZE_MAGIC);     /* Confirmation reply */

   /* Send player name, get player number */
   SDLNet_TCP_Send(sock, pname, PNAME_SIZE);
   SDLNet_TCP_Recv(sock, pno, sizeof(unsigned char));

}

void
init_player(TCPsocket sock, PLAYER *player, PICTURE *sprite)
{
   short loc;
   char *pname;

   SDLNet_TCP_Recv(sock, &player->playerno, sizeof(unsigned char));

   SDLNet_TCP_Recv(sock, &loc, sizeof(loc));
   player->x = SDLNet_Read16(&loc);

   SDLNet_TCP_Recv(sock, &loc, sizeof(loc));
   player->y = SDLNet_Read16(&loc);

   pname = malloc(PNAME_SIZE);
   SDLNet_TCP_Recv(sock, pname, PNAME_SIZE);

   player->name = pname;
   player->type = 0; /* probably not hunter, don't know yet */
   player->sprite = sprite;
   player->dead = 0;
}
