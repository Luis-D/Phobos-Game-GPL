#include "Scene.h"


void Collisions_Update()
{
    Edges_Map_2D_struct * EdgesMap = &Pho_Scene.CollisionMap;
    struct Chara * player;
    struct _Entities_LL_ * Current_Entity = Entities_Sys.Entities_LL_First;
    
    //Check collision of entities AABB with the Edges Map of the Scene;
    while(Current_Entity !=NULL) 
    {
	    player = &Current_Entity->Entity;
	    int col = Edges_Map_2D_vs_Swept(EdgesMap, &player->Movement.HitBox->AABB, Swept_AABB_vs_Line_Segment_2D_Check,Swept_AABB_2D_Response_Slide,-1.f);
	    Current_Entity = Current_Entity->Next; 
     }

    Current_Entity = Entities_Sys.Entities_LL_First;

    //Check collision of entities AABB with the triggers edges of the Scene;
    while(Current_Entity != NULL)
    {
        player = &Current_Entity->Entity;
        float Mov[4];
        memcpy(Mov,player->Movement.HitBox->AABB.Center_Position,8);
        V2V2ADD(player->Movement.HitBox->AABB.Center_Position,
        player->Movement.HitBox->AABB.Direction,&Mov[2]);

        Pho_Triggers_Test_LinkedList(&Pho_Scene.Triggers,
        Pho_Trigger_Check_as_Segment_vs_Segment,Pho_Scene.Lua_Script,
        Mov,player);
        
	    Current_Entity=Current_Entity->Next;
    }

    Current_Entity = Entities_Sys.Entities_LL_First;
 
    //Update the position once calculated the movements and collisions
    while(Current_Entity != NULL)
    {
        player = &Current_Entity->Entity;
        V2V2ADD(player->Movement.HitBox->AABB.Center_Position,player->Movement.HitBox->AABB.Direction,player->Movement.HitBox->AABB.Center_Position);
        if (player->VRAM_Instace != NULL)
        { 
            V2V2ADD(&player->VRAM_Instace->Matrix[12],player->Movement.HitBox->AABB.Center_Position,&player->VRAM_Instace->Matrix[12]);
        }
        Current_Entity = Current_Entity->Next;
    }


}
