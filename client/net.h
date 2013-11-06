#ifndef _MOT_NET_H_
#define _MOT_NET_H_

#define MAZE_MAGIC      0x6D7A
#define ADD_PLAYER      0x4E45
#define HUNTER          0x4855
#define ILLEGAL_MOV     0xF000
#define PLAYER_MOV      0x4D4F
#define PLAYER_DC       0x4443
#define PLAYER_DIE      0x4B4F
#define PLAYER_WIN      0x5749
#define SRV_BUSY        0xEEEE

#define PNAME_SIZE      32

Uint16 getshort(TCPsocket sock);
Uint32 getint(TCPsocket sock);
void getmaze(TCPsocket sock, char *pname, unsigned char *pno);
void init_player(TCPsocket sock, PLAYER *player, PICTURE *sprite);

#endif
