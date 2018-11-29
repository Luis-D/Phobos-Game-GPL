#ifndef _PHO_SCENE_H_
#define _PHO_SCENE_H_

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
    //Add a Navigation map. Add a NaviMap struct to LD_Navi_2D.h
    //Add an event handler.

    

}Pho_Scene;

int Scene_Set_Map_STLbin(char * Filename)
{
    STL_Mesh_NoNormals_Struct NMESH;
    
    if(STL_BIN_Load_NoNormals(Filename,&NMESH)==0)
    {return 0;}

    Pho_Scene.CollisionMap.EdgesBuffer = (Line_Segment_2D_Struct*)Triangles_3D_Extract_Border_Edges_to_2D_no_Z((float*) NMESH.Triangle_Array,
NMESH.Triangles_Count,&Pho_Scene.CollisionMap.Edges_Count);
 
    free (NMESH.Triangle_Array);
    return 1;
}

#endif
