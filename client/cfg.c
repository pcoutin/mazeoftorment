#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "mot.h"

static void
mt_getboolean(lua_State *L, const char *name, unsigned char *arg)
{
   lua_getglobal(L, name);

   if (!lua_isnil(L, -1))
   {
      if (!lua_isboolean(L, -1))
      {
         error(L, "`%s' should be a boolean.\n", name);
      }
      else
      {
         *arg = lua_toboolean(L, -1);
      }
   }
   lua_pop(L, 1);
}

static void
mt_getnumber(lua_State *L, const char *name, double *arg)
{
   lua_getglobal(L, name);

   if (!lua_isnil(L, -1))
   {
      if (!lua_isnumber(L, -1))
      {
         error(L, "`%s' should be a number.\n", name);
      }
      else
      {
         *arg = lua_tonumber(L, -1);
      }
   }
   lua_pop(L, 1);
}

static void
mt_getstring(lua_State *L, const char *name, char **arg)
{
   lua_getglobal(L, name);

   if (!lua_isnil(L, -1))
   {
      if (!lua_isstring(L, -1))
      {
         error(L, "`%s' should be a string.\n", name);
      }
      else
      {
         *arg = lua_tostring(L, -1);
      }
   }
   lua_pop(L, 1);
}

void
parsecfg(lua_State *L, CLC_CONFIG *config)
{
   FILE *cfile;

   /*
    * Set default values for configuration.
    */
   unsigned char isfullscreen = DEF_FULLSCREEN;
   unsigned char hwaccel      = DEF_HWACCEL;
   double win_width           = DEF_WIDTH;
   double win_height          = DEF_HEIGHT;
   config->win_flags          = 0;
   config->renderflags        = 0;
   config->luamain            = "main.lua";

   /*
    * Attempt to parse config file.
    */
   if (luaL_loadfile(L, CFG_FNAME) || lua_pcall(L, 0, 0, 0))
   {
      error(L, "Failed to parse configuration file: %s", lua_tostring(L, -1));
      return;
   }

   mt_getstring(L, "mt_luamain", &config->luamain);
   mt_getstring(L, "mt_defaultsrv", &config->defaultsrv);
   mt_getboolean(L, "mt_hwaccel", &hwaccel);
   mt_getboolean(L, "mt_fullscreen", &isfullscreen);
   mt_getnumber(L, "w_xres", &win_width);
   mt_getnumber(L, "w_yres", &win_height);

   config->win_width = win_width;
   config->win_height = win_height;

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
