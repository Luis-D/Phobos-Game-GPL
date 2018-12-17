#ifndef _LUA_LD_MATH_H
#define _LUA_LD_MATH_H

#include "libLDCC/LD_Math.h"

int l_V2Distance(lua_State *L)
{
    lua_pushnumber(L,V2Distance((float*)lua_topointer(L,1),(float*)lua_topointer(L,2)));
    return 1;
}

void Lua_LD_Math_add_registers(lua_State *L)
{
    lua_register(L,"V2Distance",l_V2Distance);    

}

#endif
