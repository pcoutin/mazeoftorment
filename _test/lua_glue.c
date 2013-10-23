#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>

static const struct luaL_reg mylib [] = {
   {"lolo", l_Lolo},
   {NULL, NULL} /* sentinel */
};

static int
l_Lolo(lua_State *L)
{
   lua_pushstring(L, "Lolo!");
   return 1;
}

//void
//init_lua()
main()
{
   int error;
   lua_State *L = lua_open();

   lua_pushcfunction(L, l_Lolo);
   lua_setglobal(L, "lolo");

   /* Open the standard Lua libraries. */
   luaL_openlibs(L);

   /* Execute a file or something */
   if (luaL_loadfile(L, "helloscript.lua"))
      fprintf(stderr, "WOW ERROR WOW: failed to load helloscript.lua\n");

   if (lua_pcall(L, 0, 0, 0))
      fprintf(stderr, "lua_pcall() failed\n");

   lua_close(L);

   return 0;
}
