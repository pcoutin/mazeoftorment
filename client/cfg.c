#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "clc.h"

void
parsecfg(lua_State *Lol, CLC_CONFIG *config)
{
   FILE *cfile;
   lua_State *L = lua_open();

   luaL_openlibs(L);

   /*
    * Set default values for configuration.
    */
   config->win_width          = DEF_WIDTH;
   config->win_height         = DEF_HEIGHT;
   config->win_fullscreen     = DEF_FULLSCREEN;
   config->win_flags          = 0;

   /*
    * Attempt to parse config file.
    */
   if (luaL_loadfile(L, CFG_FNAME) || lua_pcall(L, 0, 0, 0))
   {
      error(L, "Failed to parse configuration file: %s", lua_tostring(L, -1));
   }

   lua_getglobal(L, "mt_fullscreen");
   lua_getglobal(L, "w_xres");
   lua_getglobal(L, "w_yres");

   /*
    * Using a loop here would be nicer...
    */
   if (!lua_isboolean(L, -3))
   {
      error(L, "`mt_fullscreen' should be true or false.\n");
   }
   else if (lua_toboolean(L, -3))
   {
      config->win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
   }
      
   if (!lua_isnumber(L, -2))
   {
      error(L, "`w_xres' should be a number.\n");
   }
   else
   {
      config->win_width = (unsigned int) lua_tonumber(L, -2);
   }

   if (!lua_isnumber(L, -1))
   {
      error(L, "`w_yres' should be a number.\n");
   }
   else
   {
      config->win_height = (unsigned int) lua_tonumber(L, -1);
   }
}
