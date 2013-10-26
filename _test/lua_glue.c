#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>

static int
mt_Lolo(lua_State *L)
{
   lua_pushstring(L, "Lolo!");
   return 1;
}

static void
mt_magic()
{
   puts("this is from C");
}

static const struct luaL_reg mt_lualib [] = {
   {"lolo", mt_Lolo},
   {"magic", mt_magic},
   {NULL, NULL}
};

int
main()
{
   int error;
   lua_State *L = lua_open();

   /* Open the standard Lua libraries. */
   luaL_openlibs(L);

   /* Load our library */
   luaL_openlib(L, "mt", mt_lualib, 0);

   /* Execute a file or something */
   if (luaL_loadfile(L, "helloscript.lua"))
      fprintf(stderr, "WOW ERROR WOW: failed to load helloscript.lua\n");

   if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "lua_pcall() failed\n");

   lua_close(L);

   return 0;
}
