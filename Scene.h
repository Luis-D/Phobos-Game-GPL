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
#include "Triggers.h"

//This struct must handle the entire scene or game world.
//Its models, cameras, Entities and events
// -Luis Delgado. 2018

struct Scene_struct
{
    STL_Mesh_NoNormals_Struct MapMesh;
    Navi_Map_2D_struct NaviMap;
    Edges_Map_2D_struct CollisionMap;    
    
    lua_State * Lua_Script;
    struct Pho_Triggers_LL_ Triggers;

}Pho_Scene;

int Scene_Set_Map_STLbin(char * Filename)
{
    STL_Mesh_NoNormals_Struct * NMESH = &Pho_Scene.MapMesh;
    float * T2D = Triangle_3D_to_2D((float*) NMESH->Triangle_Array,NMESH->Triangles_Count,2);
    
    if(STL_BIN_Load_NoNormals(Filename,NMESH)==0 || T2D ==NULL)
    {return 0;}

    Pho_Scene.CollisionMap.EdgesBuffer = (Line_Segment_2D_Struct*)Triangles_3D_Extract_Border_Edges_to_2D_no_Z((float*) NMESH->Triangle_Array,NMESH->Triangles_Count,&Pho_Scene.CollisionMap.Edges_Count);


    float * tmp = Triangle_3D_to_2D((float*)NMESH->Triangle_Array,NMESH->Triangles_Count,2);

    if(!Navi_Map_2D_Generate(tmp,NMESH->Triangles_Count,&Pho_Scene.NaviMap))
    {free(tmp);return 0;}
 
    free(tmp);

    return 1;
}

int Scene_Set_Lua_Script(char * File)
{
    Pho_Scene.Lua_Script=luaL_newstate();
    luaL_openlibs(Pho_Scene.Lua_Script);
    luaL_loadfile(Pho_Scene.Lua_Script, File);
    
    Pho_Scene.Triggers.First =NULL;
    Pho_Scene.Triggers.Last = NULL;
    return 1;
}

#define Scene_Trigger_Add(A,B,Func,_Ptr) Pho_Trigger_Add(&Pho_Scene.Triggers,A,B,Func,_Ptr)
#define Scene_Trigger_Execute(Trigger,Caller,_Over) Pho_Trigger_Execute(Trigger,Pho_Scene.Lua_Script,Caller,_Over)
#define Scene_Trigger_Clear_All() Pho_Trigger_Clear_All(&Pho_Scene.Triggers)

_NavNode_2D_ * Scene_Get_Node(float * Point_2D)
{
    _NavNode_2D_ * RET = NULL;
    float * ex=(float*)Pho_Scene.MapMesh.Triangle_Array;
    for(int i=0;i<Pho_Scene.MapMesh.Triangles_Count;i++)
    {
	char found = Check_Point_in_Triangle_2D(ex,Point_2D,4);
	if(found!=1)
	{
	    //Check_Each_Segment
	    for(int o=0;o<3;o++)
	    {
		int u=o+2;
		if(o==2){u=0;}
		float Segment[4] ={ex[o],ex[o+1],ex[u],ex[u+1]};
		found = Check_Point_in_Segment_2D(Segment,Point_2D);
		if(found){break;}
	    }	
	}

	if(found)
	{
	    RET=Pho_Scene.NaviMap.Node_Array+i;
//	    printf("->(%f,%f) | %lx\n",Point_2D[0],Point_2D[1],RET);
//	    printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",ex[0],ex[1],ex[2],ex[3],ex[4],ex[5],	ex[6],ex[7],ex[8]);
	    break;
	}
	ex+=9;
    }
    return RET;
}

void Scene_Clear_All()
{

}

#endif
