#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include "mot.h"
#include "net.h"

int
killPlayer(unsigned short playerno)
{
   return 0;
}

Uint16
getshort(TCPsocket sock)
{
   Uint16 cmd;
   int recvdata = SDLNet_TCP_Recv(sock, &cmd, sizeof(cmd));

   if (recvdata != sizeof(cmd))
   {
      return 0;
   }
   return SDLNet_Read16(&cmd);
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

int
getmaze(TCPsocket sock)
{
   if (getshort(sock) != MAZE_MAGIC)
   {
      fprintf(stderr, "Bad magic number from server\n");
      return -1;
   }

   MAZE.w = getint(sock);
   MAZE.size = getint(sock);
   MAZE.h = MAZE.size / MAZE.w;

   MAZE.data = malloc(MAZE.size);

   if (SDLNet_TCP_Recv(sock, MAZE.data, MAZE.size) != MAZE.size)
   {
      fprintf(stderr, "Failed to get maze\n");
      //return -1;
   }

   return 0;
}
