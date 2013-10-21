#include <stdio.h>
#include "clc.h"
#include "inih/ini.h"

static int
cfg_handler(void *user, const char *section,
      const char *name, const char *value)
{
   CLC_CONFIG *config = (CLC_CONFIG *) user;

   if (strncmp(name, "w_xres", 6) == 0)
   {
      sscanf(value, "%d", &config->win_width);
   }
   else if (strncmp(name, "w_yres", 6) == 0)
   {
      sscanf(value, "%d", &config->win_height);
   }
   else if (strncmp(name, "w_fullscreen", 12) == 0)
   {
      config->win_fullscreen = strncmp(value, "true", 4) == 0;
   }
   else
   {
      /* inih is weird. */
      return 0;
   }
   return 1;
}

void
parsecfg(const char *fname, CLC_CONFIG *config)
{
   FILE *cfile;

   /*
    * Set default values for configuration.
    */
   config->win_width =        DEF_WIDTH;
   config->win_height =       DEF_HEIGHT;
   config->win_fullscreen =   DEF_FULLSCREEN;

   /*
    * Attempt to parse config file.
    */
   switch (ini_parse(fname, cfg_handler, config))
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
