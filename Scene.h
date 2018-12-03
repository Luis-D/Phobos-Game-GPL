#ifndef _PHO_SCENE_H_
#define _PHO_SCENE_H_

#include <luajit-2.1/luajit.h>
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

#include "libLDCC/LD_Polygon_Extract_3D.h"
#include "libLDCC/LD_STL.h"
#include "libLDCC/LD_Navi_2D.h"


#include "View.h"
#include "Characters.h"

//This struct must handle the entire scene or game world.
//Its models, cameras, Entities and events
struct Scene_struct
{
    Edges_Map_2D_struct CollisionMap;    
    Navi_Map_2D_struct NaviMap;
    STL_Mesh_NoNormals_Struct MapMesh;
    
    lua_State * Lua_Script;

}Pho_Scene;

int Scene_Set_Map_STLbin(char * Filename)
{
    STL_Mesh_NoNormals_Struct * NMESH = &Pho_Scene.MapMesh;
    float * T2D = Triangle_3D_to_2D((float*) NMESH->Triangle_Array,NMESH->Triangles_Count,2);
    
    if(STL_BIN_Load_NoNormals(Filename,NMESH)==0 || T2D ==NULL)
    {return 0;}

    Pho_Scene.CollisionMap.EdgesBuffer = (Line_Segment_2D_Struct*)Triangles_3D_Extract_Border_Edges_to_2D_no_Z((float*) NMESH->Triangle_Array,NMESH->Triangles_Count,&Pho_Scene.CollisionMap.Edges_Count);


    if(!Navi_Map_2D_Generate((float*)NMESH->Triangle_Array, NMESH->Triangles_Count,&Pho_Scene.NaviMap))
    {return 0;}
 
    return 1;
}

int Scene_Set_Lua_Script(char * File)
{
    Pho_Scene.Lua_Script=luaL_newstate();
    luaL_openlibs(Pho_Scene.Lua_Script);
    luaL_loadfile(Pho_Scene.Lua_Script, File);
    return 1;
}

#endif
