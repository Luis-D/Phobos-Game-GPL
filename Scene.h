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

#define FunctionNameSize 8
struct Scene_struct
{
    Edges_Map_2D_struct CollisionMap;    
    Navi_Map_2D_struct NaviMap;
    STL_Mesh_NoNormals_Struct MapMesh;
    
    lua_State * Lua_Script;

    struct _Scene_Trigger_LL_
    {
	int FLAG;
	float Point_A[2];
	float Point_B[2];
	
	struct _Scene_Trigger_LL_ * Next;
	void * Parameter_Pointer;
	
	char Function[FunctionNameSize]; //Function names can only have 7 ASCII symbols plus a \0;
    } * Trigger_First;
    struct _Scene_Trigger_LL_ * Trigger_Last;
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
    
    Pho_Scene.Trigger_First =NULL;
    Pho_Scene.Trigger_Last = NULL;
    return 1;
}

struct _Scene_Trigger_LL_ * Scene_Trigger_Add
(float * Point_A,float * Point_B,char * Function_Name,void * Parameters_pointer)
{
    if(Pho_Scene.Trigger_Last==NULL)
    {
	Pho_Scene.Trigger_First = (struct _Scene_Trigger_LL_ *) malloc(sizeof(struct _Scene_Trigger_LL_));
	Pho_Scene.Trigger_Last = Pho_Scene.Trigger_First;
    } 
    else
    {
	Pho_Scene.Trigger_Last->Next=(struct _Scene_Trigger_LL_*)malloc(sizeof(struct _Scene_Trigger_LL_));
	Pho_Scene.Trigger_Last = Pho_Scene.Trigger_Last->Next;
    }
    Pho_Scene.Trigger_Last->Next=NULL;

    memcpy(Pho_Scene.Trigger_Last->Function,Function_Name,FunctionNameSize);
    Pho_Scene.Trigger_Last->Function[FunctionNameSize-1]=0; //<- Set \0 as the last character
    Pho_Scene.Trigger_Last->FLAG=0; 
    if(Point_A!=NULL){memcpy(Pho_Scene.Trigger_Last->Point_A,Point_A,sizeof(float)*2);}
    if(Point_B!=NULL){memcpy(Pho_Scene.Trigger_Last->Point_B,Point_B,sizeof(float)*2);}

    return Pho_Scene.Trigger_Last;
}

void Scene_Trigger_Clear_All()
{
    struct _Scene_Trigger_LL_* Exp = Pho_Scene.Trigger_First;
    struct _Scene_Trigger_LL_* tmp;    

    Pho_Scene.Trigger_First=NULL;
    Pho_Scene.Trigger_Last=NULL;
    
    while(Exp!=NULL)
    {
	tmp = Exp->Next;
	free(Exp);
	Exp = tmp;	
    }
}

void Scene_Trigger_Execute(struct _Scene_Trigger_LL_ * Trigger)
{
    lua_getglobal(Pho_Scene.Lua_Script,Trigger->Function); 
    lua_pushlightuserdata(Pho_Scene.Lua_Script,Trigger->Parameter_Pointer);
    lua_pcall(Pho_Scene.Lua_Script,1,0,0);
}   

#endif
