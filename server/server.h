#ifndef _MOTSRV_H_
#define _MOTSRV_H_

#define MOTSRV_ADDR     "127.0.0.1"
#define MOTSRV_PORT     "6666"
#define BACKLOG         8

#define PNAMELEN        32
#define MAX_PLAYERNUM   32

#define MAZE_MAGIC      0x6D7A
#define ADD_PLAYER      0x4E45
#define HUNTER          0x4855
#define ILLEGAL_MOV     0xF000
#define PLAYER_MOV      0x4D4F
#define PLAYER_DC       0x4443
#define PLAYER_DIE      0x4B4F
#define PLAYER_WIN      0x5749
#define SRV_BUSY        0xEEEE

#define _MOT_SERVER

/* Player linked list stuff */

typedef struct _player Player;
typedef struct _player_set Player_set;

struct _player
{
    char *name;
    short x, y;

    int fd;
    unsigned char playerno;
    unsigned char type;        // 0 is prey, 1 is predator
    unsigned char dead;

    Player *next;
    Player *prev;
};

struct _player_set
{
    Player *first;
    Player *last;
    Player *cur;
    int last_pno;
};

void begin_game(Player_set *pset);
int sendMov(int psock, short int movepno, int x, int y);
void broadcast_disconnect(Player_set *pset, int fd, int death);

Player_set *init_pset();
void free_pset(Player_set *p);
void add_player(Player_set *set);
void rm_player(Player_set *set, Player *p);
Player *player_byfd(Player_set *s, int fd);
Player *player_byindex(Player_set *s, int fd);
void print_pset(Player_set *set);
Player *check_collision(Player_set *pset, Player *node);
void pset_map(Player_set *s,
      void (*func)(Player *p, int a, unsigned short sig), int fd,
      unsigned short sig);

int mrand(int floor, int ceil);
void handle_connecting_player(int newfd, Player_set *pset);

#endif
