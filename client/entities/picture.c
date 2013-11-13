#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "entities.h"
#include "../mot.h"

PICTURE
loadPic(char *path)
{
   SDL_Surface *s;
   SDL_Texture *tex;
   PICTURE pic;

   if ((s = IMG_Load(path)) == NULL)
   {
      fprintf(stderr, "Failed to load image\n");
      exit(1);
   }

   tex = SDL_CreateTextureFromSurface(renderer, s);
   pic.rect.x = 0;
   pic.rect.y = 0;
   pic.rect.w = s->w;
   pic.rect.h = s->h;
   pic.texture = tex;

   SDL_FreeSurface(s);

   return pic;
}

void
drawPic(PICTURE pic, int x, int y)
{
   pic.rect.x = x;
   pic.rect.y = y;
   SDL_RenderCopy(renderer, pic.texture, NULL, &pic.rect);
}
