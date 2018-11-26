#ifndef PHO_LUA_L
#define PHO_LUA_L

#include "luajit-2.1/luajit.h"
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

#include "Scene.h"

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

lua_State * Lua_State_Init()
{
	lua_State * L;
	L = luaL_newstate();
	luaL_openlibs(L);
	return L;
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

int l_Pho_Model_add(lua_State * L)
{
	IQM_RAW_Struct *  _Model_ = (IQM_RAW_Struct *) malloc(sizeof(IQM_RAW_Struct));
      	if(IQM_Load_File(_Model_,lua_tostring(L,1)) ==-1){printf("ERROR::MODEL\n");}
	LD_Model_Object_Struct * Model = LD_3D_ModelsLinkedList_Append((&_Model_->VertexCount));
	free(_Model_);
	lua_pushlightuserdata(L,Model);
	return 1;
}

int l_InstanceBuffer_Set_Capacity(lua_State *L)
{
	printf("VRAM Instances: %ld\n",lua_tointeger(L,1));
	LD_3D_InstaceBuffer_Set_capacity(lua_tointeger(L,1));
	return 1;
}

int l_VRAMBuffers_allocate(lua_State *L)
{
	printf("VRAM BUFFERS: %ld\n",lua_tointeger(L,1));
	LD_3D_VRAMBuffer_allocate(lua_tointeger(L,1));
	return 1;
}

int l_VRAM_Instance_Create(lua_State *L)
{
	//printf("Model: \n", (LD_Model_Object_Struct*)lua_topointer(L,2));
	LD_3D_Fill_Instance(&LD_3D.InstacesBuffer[lua_tointeger(L,1)],(LD_Model_Object_Struct*)lua_topointer(L,2));

	lua_pushlightuserdata(L,&LD_3D.InstacesBuffer[lua_tointeger(L,1)]);
	return 1;
}

int l_Fill_VRAM_Buffer(lua_State *L)
{
	LD_3D_Fill_VRAMBuffer(&LD_3D.VRAMBuffer[lua_tointeger(L,1)],&LD_3D.InstacesBuffer[lua_tointeger(L,2)],
		&LD_3D.InstacesBuffer[lua_tointeger(L,3)],lua_tointeger(L,4));
	return 1;
}


void Lua_add_registers(lua_State * L)
{
	//VRAM_Buffer_fill(int Buffer_ID_number, int First_Instance_ID, int Last_Instance_ID, char Static_Flag);
	lua_register(L,"VRAM_Buffer_fill",l_Fill_VRAM_Buffer);
	//VRAM_Instance_Create(int Instance_ID_Number, void * Model)
	lua_register(L,"VRAM_Instance_Create",l_VRAM_Instance_Create);
	lua_register(L,"VRAM_Instances",l_InstanceBuffer_Set_Capacity);
	lua_register(L,"VRAM_Buffers",l_VRAMBuffers_allocate);
    lua_register(L,"Camera_add",l_Pho_Camera_add);
	lua_register(L,"Model_add",l_Pho_Model_add);	
}

#endif
