#ifndef _PHO_COLLISION_H_
#define _PHO_COLLISION_H_

#include "Scene.h"
#include "Characters.h"

void Collisions_Update()
{
    Edges_Map_2D_struct * EdgesMap = &Pho_Scene.CollisionMap;
    struct _AABB_LL_ * Current_AABB = Entities_Sys.AABB_LL_First;
    
    //Check collision of entities AABB with the Edges Map of the Scene;
    while(Current_AABB !=NULL) 
    {
	    int col = Edges_Map_2D_vs_Swept(EdgesMap, &Current_AABB->AABB, Swept_AABB_vs_Line_Segment_2D_Check,Swept_AABB_2D_Response_Slide,-1.f);
	    Current_AABB = Current_AABB->Next; 
     }

    Current_AABB = Entities_Sys.AABB_LL_First;
    //Check collision of entities AABB with the triggers edges of the Scene;
    while(Current_AABB != NULL)
    {
        float Mov[4];
        memcpy(Mov,Current_AABB->AABB.Center_Position,8);
        V2V2ADD(Current_AABB->AABB.Center_Position,
        Current_AABB->AABB.Direction,&Mov[2]);

        Pho_Triggers_Test_LinkedList(&Pho_Scene.Triggers,
        Pho_Trigger_Check_as_Segment_vs_Segment,Pho_Scene.Lua_Script,
        Mov,Current_AABB->Owner);
        
	    Current_AABB=Current_AABB->Next;
    }

    Current_AABB = Entities_Sys.AABB_LL_First;
    //Update the position once calculated the movements and collisions
    while(Current_AABB != NULL)
    {
        V2V2ADD(Current_AABB->AABB.Center_Position,Current_AABB->AABB.Direction,Current_AABB->AABB.Center_Position);
        Current_AABB = Current_AABB->Next;
    }
}

#endif
