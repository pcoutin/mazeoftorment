#ifndef _MOT_NET_H_
#define _MOT_NET_H_

#define MAZE_MAGIC     0x6D7A
#define ADD_PLAYER     0x4E45
#define HUNTER         0x4855
#define ILLEGAL_MOV    0xF000
#define PLAYER_MOV     0x4D4F
#define PLAYER_DC      0x4443
#define PLAYER_DIE     0x4B4F
#define PLAYER_WIN     0x5749

Uint16 getshort(TCPsocket sock);
Uint32 getint(TCPsocket sock);
int getmaze(TCPsocket sock);

#endif
