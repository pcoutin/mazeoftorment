#ifndef _MOT_ENTITIES_H
#define _MOT_ENTITIES_H

typedef struct _mot_picture PICTURE;
typedef struct _mot_player PLAYER;

struct _mot_picture
{
   SDL_Texture *texture;
   SDL_Rect rect;
};

struct _mot_player
{
   char *name;
   PICTURE *sprite;

   short x, y;                // location in maze tiles

   unsigned char type;        // 0 is prey, 1 is predator
   unsigned char playerno;
   unsigned char dead;

   PLAYER *next;
   PLAYER *prev;

};

void init_player(TCPsocket sock, PLAYER *player);

void drawPlayer(PLAYER *);

void local_player_update(TCPsocket sock, PLAYER *, PLAYER *, const Uint8 *);

/*
 * Move the player `p' TO a specified x, y location (assigns, doesn't
 * translate).
 */
void movePlayer(PLAYER *p, short newx, short newy);

/*
 * Reset the maze cell that the player is on...
 */
void clearPlayer(PLAYER *);

/*
 * Return the player with pnode `pnum' from the player linked list
 * starting with `node'.
 */
PLAYER *choose_player(PLAYER *node, unsigned char pnum);

/*
 * Free the player `temp' after linking the two players next to `temp'
 * together. Also erases it from the screen.
 */
void removep(PLAYER *temp);

/*
 * On the linked list starting with `node', find the player with pnode
 * `hpno', and set it to be of hunter type.
 */
void choose_hunter(PLAYER *node, unsigned char hpno);

/*
 * Add an initialized player "object" to the player linked list containing
 * `node'.
 */
void add_player(PLAYER *node, PLAYER *newp);

/*
 * Keep receiving players from the server until it's done and it chooses a
 * hunter, adding them to the player linked list containing `node'.
 */
unsigned char addp(PLAYER *node, TCPsocket srv_sock);

/*
 * GLOBALS
 */

// Black tile to erase players as they move, and hunter/prey sprites.
PICTURE black, psprite, hsprite;

#endif
