#ifndef _PHO_AI_H
#define _PHO_AI_H

#include "Scene.h"
#include "Characters.h"
#include "Collision.h" //<- this also includes Scene.h and Characters.h

//This header contains rountines to execute the AI of the Entities.
//- Luis Delgado. 2018

void Entity_AI_Evaluate_Path(_Entities_LL_* Entity,float * Point2D_Return)
{
    //Compute the most efficient way through the path finding the nearest node that doesn't triggers collisions
    float * Destiny = Entity->Entity.AI.PathFinder.Path->Destiny;
//    printf("%f,%f\n",Destiny[0],Destiny[1]); 

    //Check if going directly to the Destiny is posible//
    Swept_AABB_2D_Struct AABB; //<- It's a modified copied of the Entity's AABB
    Swept_AABB_2D_Struct * ptr_AABB = &Entity->Entity.Movement.HitBox->AABB;
    memcpy(&AABB,ptr_AABB,sizeof(Swept_AABB_2D_Struct));
    V2V2SUB(Destiny,ptr_AABB->Center_Position,AABB.Direction);
    memcpy(Point2D_Return,ptr_AABB->Center_Position,8); //<- By Default it will return the position

///	    printf("\t (%x,%x), (%x,%x)\n",*(int*)&AABB.Center_Position[0],*(int*)&AABB.Center_Position[1],*(int*)&AABB.Direction[0],*(int*)&AABB.Direction[1]);
    if(!Edges_Map_2D_vs_Swept(&Pho_Scene.CollisionMap,&AABB,Swept_AABB_vs_Line_Segment_2D_Check,NULL,-1.f))
    {

//	printf("Going Directly to (%f,%f)\n",Destiny[0],Destiny[1]);
//	    printf("\t (%f,%f), (%f,%f)\n",AABB.Center_Position[0],AABB.Center_Position[1],AABB.Direction[0],AABB.Direction[1]);

	memcpy(Point2D_Return,Destiny,8);
    }
    else //If going directly to the destiny isn't possible...
    {
	//Compute the most efficient node that does not triggers a collision
	//Starting from the end of the queue of points
	_NavNode_2D_LL * Curr = Entity->Entity.AI.PathFinder.Path->Last;
	
	while(Curr!=NULL)
	{
	    Destiny = &Curr->Node->x;
	    V2V2SUB(Destiny,AABB.Center_Position,AABB.Direction);
 
	    char col = Edges_Map_2D_vs_Swept(&Pho_Scene.CollisionMap,&AABB,Swept_AABB_vs_Line_Segment_2D_Check,NULL,-1.f); 
	    if(0==col)
	    {
		memcpy(Point2D_Return,Destiny,8);
		break;
	    }
	    Curr=Curr->Last;
	}
    }
}

char Entity_AI_Sensor_Check_Entity(_Entities_LL_* Entity,_Entities_LL_*Test)
{
    return Sensor_Check_as_FOV_2D(&Entity->Entity.Sensor.Sensor,
    Entity->Entity.Movement.HitBox->AABB.Center_Position,
    Entity->Entity.Movement.Direction_Degree,
    Test->Entity.Movement.HitBox->AABB.Center_Position); 
}

#define Sensor_Entities_Buffer 0
#define Sensor_Drops_Buffer 1
#define Sensor_Interactives_Buffer 2
char Entity_AI_Sensor_Update(_Entities_LL_* Entity)
{
    char SomethingFound=0;
    struct Sensor_struct * Sensor = &Entity->Entity.Sensor.Sensor;


    //Check for all the others entities//
    _Entities_LL_*Ex = Entities_Sys.Entities_LL_First;

    while(Ex!=NULL)
    {
	if(Entity_AI_Sensor_Check_Entity(Entity,Ex))
	{
	    SomethingFound=1;
	    Sensor_add(Sensor,Sensor_Entities_Buffer,Ex);
	}
	Ex=Ex->Next;
    }


    return SomethingFound;
}

void Characters_AI_Update()
{
    struct _AI_LL_ * Current_AI = Entities_Sys.AI_LL_First; 
      
    while(Current_AI !=NULL)  
    {
	Entity_AI_Script_Execute(Current_AI,NULL);
	Current_AI = Current_AI->Next;
    }
}

#endif
