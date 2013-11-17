#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <string.h>
#include "../common/inih/ini.h"
#include "entities/entities.h"
#include "mot.h"

static int
cfg_handler(void *user, const char *sec, const char *name, const char *value)
{
   CLC_CONFIG *config = (CLC_CONFIG *) user;

   if (!strcmp(name, "mt_defaultsrv"))
   {
      config->defaultsrv = strdup(value);
   }
   else if (!strcmp(name, "mt_defaultport"))
   {
      config->defaultport = strdup(value);
   }
   else if (!strcmp(name, "mt_hwaccel") && !strcmp(value, "true"))
   {
      config->renderflags |= SDL_RENDERER_ACCELERATED;
   }
   else if (!strcmp(name, "mt_hwaccel") && !strcmp(value, "false"))
   {
      config->renderflags |= SDL_RENDERER_SOFTWARE;
   }
   else if (!strcmp(name, "mt_hwaccel"))
   {
      fprintf(stderr,
            "On config.ini: mt_hwaccel should be true or false.\n");
   }
   else if (!strcmp(name, "mt_fullscreen") && !strcmp(value, "true"))
   {
      config->win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
   }
   else if (!strcmp(name, "mt_fullscreen") && !strcmp(value, "false"))
   {
      return 1;
   }
   else if (!strcmp(name, "mt_fullscreen"))
   {
      fprintf(stderr,
            "On config.ini: mt_fullscreen should be true or false.\n");
   }
   else if (!strcmp(name, "w_xres"))
   {
      config->win_width = atoi(value);
   }
   else if (!strcmp(name, "w_yres"))
   {
      config->win_height = atoi(value);
   }
   else
   {
      puts("oh no");
      return 0;
   }
   return 1;
}

void
parsecfg(CLC_CONFIG *config)
{
   /*
    * Set default values for configuration.
    */
   config->win_width          = DEF_WIDTH;
   config->win_height         = DEF_HEIGHT;
   config->win_flags          = 0;
   config->renderflags        = 0;
   config->defaultsrv         = "localhost";
   config->defaultport        = "6666";

   /*
    * Attempt to parse config file.
    */
   switch (ini_parse(CFG_FNAME, cfg_handler, config))
   {
      case 0:
         break;
      case -1:
         fprintf(stderr, "Error opening config file %s\n", CFG_FNAME);
         break;
      default:
         fprintf(stderr, "Error parsing config file %s\n", CFG_FNAME);
   }
}
