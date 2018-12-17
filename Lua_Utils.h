#ifndef _LUA_UTILS_H_
#define _LUA_UTILS_H_

int lua_print2D(lua_State *L)
{
    float Point2D[2];
    memcpy(Point2D,lua_topointer(L,1),8);
    printf("(%f,%f)\n",Point2D[0],Point2D[1]);
    return 1;
}

void Lua_Utils_add_registers(lua_State *L)
{
    lua_register(L,"print2D",lua_print2D);
}

#endif
