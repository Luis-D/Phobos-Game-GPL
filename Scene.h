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

    

};



#endif
