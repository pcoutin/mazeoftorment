#include <stdio.h>
#include "mot.h"

static char *cfgfile;

static void mt_cfgread(FILE *f);
static char *mt_find(char *vname);
static void mt_getstr(char *vname, char **out);
static void mt_getbool(char *vname, unsigned char *out);
static void mt_getint(char *vname, int *out);

void
parsecfg(CLC_CONFIG *config)
{
   FILE *f;
   size_t fsize;

   /*
    * Set default values for configuration.
    */
   unsigned char isfullscreen = DEF_FULLSCREEN;
   unsigned char hwaccel      = DEF_HWACCEL;
   config->win_width          = DEF_WIDTH;
   config->win_height         = DEF_HEIGHT;
   config->win_flags          = 0;
   config->renderflags        = 0;
   config->defaultsrv         = "localhost";
   config->defaultport        = "6666";

   return;

   /*
    * Attempt to parse config file.
    */
   if (fopen(CFG_FNAME, "r") == NULL)
   {
      perror("Failed to load config file " CFG_FNAME);
      return;
   }

   /*
    * Read it all into a buffer.
    */
   fseek(f, 0, SEEK_END);
   fsize = ftell(f);
   fseek(f, 0, SEEK_SET);

   cfgfile = malloc(fsize + 1);
   fread(config, fsize, 1, f);
   fclose(f);

   *(cfgfile + fsize) = 0;

   mt_getstr("mt_defaultsrv", &config->defaultsrv);
   mt_getbool("mt_hwaccel", &hwaccel);
   mt_getbool("mt_fullscreen", &isfullscreen);
   mt_getint("w_xres", &config->win_width);
   mt_getint("w_yres", &config->win_height);

   if (isfullscreen)
   {
      config->win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
   }

   if (hwaccel)
   {
      config->renderflags |= SDL_RENDERER_ACCELERATED;
   }
   else
   {
      config->renderflags |= SDL_RENDERER_SOFTWARE;
   }
}

static char *mt_find(char *vname)
{
   while ("MAGIC")
   {
   }
}
