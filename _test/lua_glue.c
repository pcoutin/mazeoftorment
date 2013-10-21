#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>

//void
//init_lua()
main()
{
   int error;
   lua_State *L = lua_open();

   /* Open the standard Lua libraries. */
   luaL_openlibs(L);

   /* Execute a file or something */
   if (luaL_loadfile(L, "helloscript.lua"))
      fprintf(stderr, "WOW ERROR WOW\n");

   printf("In C, calling Lua\n");

   if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "lua_pcall() failed\n");

   printf("Back in C again\n");

   lua_close(L);

   return 0;
}
