#include "luajit-2.1/luajit.h"
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

void bail(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
	exit(1);
}

lua_State * Lua_Thread(lua_State **L)
{
    *L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(*L);                           /* Load Lua libraries */
    lua_State* thread = lua_newthread(*L);
    return thread;
}

int Lua_Execute(lua_State * Lua_State,const char * Script_file_path)
{
    if (luaL_loadfile(Lua_State, Script_file_path))    /* Load but don't run the Lua script */
    {bail(Lua_State, "luaL_loadfile() failed");}      /* Error out if file can't be read */

                 /* Run the loaded Lua script */

    return lua_resume(Lua_State, 0);
}

void Lua_Close(lua_State * L){lua_close(L);}

int l_Pho_Camera_add(lua_State * L)
{
    Pho_Camera_add(lua_tonumber(L,1),lua_tonumber(L,2),lua_tonumber(L,3),
    lua_tonumber(L,4),lua_tonumber(L,5),lua_tonumber(L,6),lua_tonumber(L,7),
    lua_tonumber(L,8));
    return 1;
}

void Lua_add_registers(lua_State * L)
{
    lua_register(L,"Camera_add",l_Pho_Camera_add);
}