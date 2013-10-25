/*
 * Make some kind of struct thing compatible with lua for the 2 player
 * types? (prey and hunter)
 * Implement prey and hunter more in lua? Movement is the same for both so
 * just implement that in C. Game mechanics, winning, etc in lua.
 *
 * HAVE error messages for not being able to open an image instead of oops
 * segfault who cares if couldn't open
 *
 * For lua tables, store the player object on the key but put true or
 * whatever in the value. Or it'll be slow.
 */

typedef struct
{
   PICTURE pic;
   unsigned int x, y;
   void (*update) (Player *self);
} Player;

/*
 * draws and updates player.
 */
static void
update(Player *self)
{
   Uint8 *state = SDL_GetKeyboardFromState(int *numkeys);

   /*
    * Move if a key was pressed.
    */
   if (state[SDL_SCANCODE_UP])
   {
      self->y--; // not really
   }
   if (state[SDL_SCANCODE_DOWN])
   {
   }
   if (state[SDL_SCANCODE_LEFT])
   {
   }
   if (state[SDL_SCANCODE_RIGHT])
   {
   }
}
