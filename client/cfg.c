#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "clc.h"

static void
mt_getbool(lua_State *L, const char *name, unsigned char *arg)
{
   lua_getglobal(L, name);

   if (!lua_isnil(L, -1))
   {
      if (!lua_isstring(L, -4))
      {
         error(L, "`mt_luamain' should be a string.\n");
      }
      else if (!lua_isnil(L, -4))
      {
         config->luamain = lua_tostring(L, -4);
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
   config->win_width          = DEF_WIDTH;
   config->win_height         = DEF_HEIGHT;
   config->win_flags          = 0;
   config->luamain            = "main.lua";

   /*
    * Attempt to parse config file.
    */
   if (luaL_loadfile(L, CFG_FNAME) || lua_pcall(L, 0, 0, 0))
   {
      error(L, "Failed to parse configuration file: %s", lua_tostring(L, -1));
      return;
   }

   /* Reset the lua stack */
   lua_settop(L, 0);

   lua_getglobal(L, "mt_defaultsrv");
   lua_getglobal(L, "mt_luamain");
   lua_getglobal(L, "mt_fullscreen");
   lua_getglobal(L, "w_xres");
   lua_getglobal(L, "w_yres");

   /*
    * I wish this could be less ugly...
    * If you try checking if a nil value is a string, lua (5.1 at least)
    * gives an "unknown error" and makes the program exit.
    */

   if (!lua_isnil(L, -4))
   {
      if (!lua_isstring(L, -4))
      {
         error(L, "`mt_luamain' should be a string.\n");
      }
      else
      {
         config->luamain = lua_tostring(L, -4);
      }
   }

   if (!lua_isnil(L, -3))
   {
      if (!lua_isboolean(L, -3))
      {
         error(L, "`mt_fullscreen' should be true or false.\n");
      }
      else if (lua_toboolean(L, -3))
      {
         config->win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
      }
   }
      
   if (!lua_isnil(L, -2))
   {
      if (!lua_isnumber(L, -2))
      {
         error(L, "`w_xres' should be a number.\n");
      }
      else
      {
         config->win_width = (unsigned int) lua_tonumber(L, -2);
      }
   }

   if (!lua_isnil(L, -1))
   {
      if (!lua_isnumber(L, -1))
      {
         error(L, "`w_yres' should be a number.\n");
      }
      else
      {
         config->win_height = (unsigned int) lua_tonumber(L, -1);
      }
   }

   lua_settop(L, 0);
}
