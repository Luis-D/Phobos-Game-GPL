/*This header contains a generic structure for movable entities
 * This Entity struct can be used to create different things such as enemies or players
 * This Entity struct have a void* Data so an entity can have a buffer with data
 * When an entity is deleted, its Data buffer is also deleted by a free().
 * This Entity struct have a Sensor struct so entities can interact with the world
*/
//- Luis Delgado. 2018

#ifndef __Characters_H_
#define __Characters_H_

#include "libLDCC/LD_Collision_2D.h"
#include "libLDCC/LD_Navi_2D.h"
#include "Sensors.h"


#define FunctionNameSize 8 //Names can only have 7 ASCII symbols plus a \0
#define Entity_SensorBufferSize 3 //Number of buffers for the sensors,
//Each buffer shall be for a diferent kind of data.

struct Entities_Sys_Struct
{
    int Entities_Count;

    struct _AI_LL_
    {
	lua_State * Lua;
	char Function[FunctionNameSize]; 
	void * Caller;
 	void * Parameter_Pointer;
	struct _AI_LL_ * Next;
	struct _AI_LL_ * Last;
    }*AI_LL_First;
    struct _AI_LL_ * AI_LL_Last;
    
    struct _AABB_LL_
    {
        Swept_AABB_2D_Struct AABB;
	void * Owner;
	struct _AABB_LL_ * Next;
        struct _AABB_LL_ * Last;
    } * AABB_LL_First;
    struct _AABB_LL_ * AABB_LL_Last;

    struct _Entities_LL_
    {
        struct Chara 
        { 
	    int ID; //<- and ID to identify the Entity
            void * Data; //Each Entity can have a pointer to a buffer with it's unique data	    
	
	    struct __Sensor_
	    {
		void * Lock;
		struct Sensor_struct Sensor;
	    }Sensor;  

	    struct __AI_
	    {
		struct _AI_LL_ * Script;
		struct _AI_PathFinder__
		{
		    Path_2D_struct * Path;
		}PathFinder;
	    }AI;

	    float Instance_Scale;
            struct LD_Instance_Struct * VRAM_Instace;

            struct __Movement_
            {   
                    float Forward, Turn; 
                    float Turn_Speed;
                    float Direction_Degree;
                    float Mov_Speed;
                    struct _AABB_LL_ * HitBox;
                    float z;
            } Movement; 
	} Entity;

        struct _Entities_LL_ * Next;
        struct _Entities_LL_ * Last;
    } * Entities_LL_First;
    struct _Entities_LL_ * Entities_LL_Last;

}Entities_Sys;
typedef struct _Entities_LL_ _Entities_LL_;
typedef struct _AI_LL_ _AI_LL_;
typedef struct Entity_Routine Entity_Routine;

void Entities_Sys_init()
{
    Entities_Sys.Entities_Count=0;
    Entities_Sys.Entities_LL_First=NULL;
    Entities_Sys.Entities_LL_Last=NULL;
   
    Entities_Sys.AABB_LL_First=NULL;
    Entities_Sys.AABB_LL_Last=NULL;
}


_Entities_LL_ * Entity_Create(int ID,float Pos_X, float Pos_Y,float Direction_Degree,
 float Movement_Speed, float Turn_Speed,float HitBox_Half_Length,lua_State * LUA_STATE,char * Main_Routine)
{
    if(Entities_Sys.Entities_LL_Last == NULL)
    {
        Entities_Sys.Entities_LL_First = (struct _Entities_LL_*) 
        malloc(sizeof(struct _Entities_LL_));
        Entities_Sys.AABB_LL_First = (struct _AABB_LL_*) 
        malloc(sizeof(struct _AABB_LL_));
        Entities_Sys.AI_LL_First = (struct _AI_LL_*) 
        malloc(sizeof(struct _AI_LL_));

        if(Entities_Sys.Entities_LL_First==NULL || 
        Entities_Sys.AABB_LL_First==NULL || 
	Entities_Sys.AI_LL_First==NULL)
	{return NULL;}

        Entities_Sys.Entities_LL_First->Last=NULL;
        Entities_Sys.Entities_LL_Last = Entities_Sys.Entities_LL_First;

        Entities_Sys.AABB_LL_First->Last=NULL;
        Entities_Sys.AABB_LL_Last = Entities_Sys.AABB_LL_First;
        
	Entities_Sys.AI_LL_First->Last=NULL;
        Entities_Sys.AI_LL_Last = Entities_Sys.AI_LL_First;
    }
    else
    {
        Entities_Sys.Entities_LL_Last->Next = (struct _Entities_LL_*) 
        malloc(sizeof(struct _Entities_LL_));

        Entities_Sys.AABB_LL_Last->Next = (struct _AABB_LL_*) 
        malloc(sizeof(struct _AABB_LL_));
        
	Entities_Sys.AI_LL_Last->Next = (struct _AI_LL_*) 
        malloc(sizeof(struct _AI_LL_));

        if(Entities_Sys.Entities_LL_Last->Next==NULL || 
        Entities_Sys.AABB_LL_Last->Next==NULL || 
	Entities_Sys.AI_LL_Last->Next==NULL)
	{return NULL;}

        struct _Entities_LL_ * tmp2 = Entities_Sys.Entities_LL_Last->Next;
        tmp2->Last=Entities_Sys.Entities_LL_Last;
        Entities_Sys.Entities_LL_Last = tmp2;

        struct _AABB_LL_ * tmp3 = Entities_Sys.AABB_LL_Last->Next;
        tmp3->Last=Entities_Sys.AABB_LL_Last;
        Entities_Sys.AABB_LL_Last = tmp3;

        struct _AI_LL_ * tmp4 = Entities_Sys.AI_LL_Last->Next;
        tmp4->Last=Entities_Sys.AI_LL_Last;
        Entities_Sys.AI_LL_Last = tmp4;
    }
    Entities_Sys.AI_LL_Last->Lua = LUA_STATE;
    Entities_Sys.AI_LL_Last->Next=NULL; 
    Entities_Sys.AABB_LL_Last->Next=NULL;
    Entities_Sys.AABB_LL_Last->Owner = (void*) Entities_Sys.Entities_LL_Last;
    Entities_Sys.Entities_LL_Last->Next=NULL;
    Entities_Sys.Entities_LL_Last->Entity.Movement.HitBox = Entities_Sys.AABB_LL_Last;
    Entities_Sys.Entities_LL_Last->Entity.AI.Script = Entities_Sys.AI_LL_Last;
    Entities_Sys.Entities_LL_Last->Entity.AI.PathFinder.Path=NULL; 

    struct Chara * tmp = &Entities_Sys.Entities_LL_Last->Entity;
    tmp->Movement.Forward=0;tmp->Movement.Turn=0;
    tmp->Movement.Turn_Speed = Turn_Speed;
    tmp->Movement.Direction_Degree = Direction_Degree;
    tmp->Movement.Mov_Speed = Movement_Speed;
    tmp->Movement.HitBox->AABB.Center_Position[0] = Pos_X;
    tmp->Movement.HitBox->AABB.Center_Position[1] = Pos_Y;
    tmp->Movement.HitBox->AABB.Direction[0] = 0;
    tmp->Movement.HitBox->AABB.Direction[1] = 0;
    tmp->Movement.HitBox->AABB.Half_Extent[0] = HitBox_Half_Length;
    tmp->Movement.HitBox->AABB.Half_Extent[1] = HitBox_Half_Length;
    tmp->Movement.z=0;
    tmp->Data=NULL;
    tmp->VRAM_Instace=NULL;
    tmp->Instance_Scale=1.f;
    tmp->ID = ID;
    Sensor_allocate(&tmp->Sensor.Sensor,Entity_SensorBufferSize);

    if(LUA_STATE != NULL)
    {
	memcpy(tmp->AI.Script->Function,Main_Routine,FunctionNameSize); //<- this is OK
	tmp->AI.Script->Function[7]=0;//<- Adding a \0 at the end
	tmp->AI.Script->Caller= (void*)tmp;
    }

    Entities_Sys.Entities_Count++;

    return Entities_Sys.Entities_LL_Last;
}

static void _HitBox_Delete(struct _AABB_LL_ * AABB_LL)
{
    struct _AABB_LL_ * N;
    struct _AABB_LL_ * L;

    N = AABB_LL->Next;
    L = AABB_LL->Last;

    L->Next=N;
    N->Last=L;

    free(AABB_LL);
}

void _ArtificialInteligence_Delete(struct _AI_LL_* AI_LL)
{
    struct _AI_LL_ * N;
    struct _AI_LL_ * L;

    N = AI_LL->Next;
    L = AI_LL->Last;

    L->Next=N;
    N->Last=L;

    free(AI_LL);
}

void * Entity_Delete(struct _Entities_LL_ * Entity_ptr,char Keep_Data)
{
    struct _Entities_LL_ * N;
    struct _Entities_LL_ * L;
    void * RET = NULL;

    N = Entity_ptr->Next;
    L = Entity_ptr->Last;

    L->Next=N;
    N->Last=L;

    _HitBox_Delete(Entity_ptr->Entity.Movement.HitBox);
    _ArtificialInteligence_Delete(Entity_ptr->Entity.AI.Script);

    if(Entity_ptr->Entity.Data!=NULL || Keep_Data ==0)
    {free (Entity_ptr->Entity.Data);}
    else{RET = Entity_ptr->Entity.Data;}


    free(Entity_ptr);

    Entities_Sys.Entities_Count--;

    return RET;
}

void Entity_set_Model_Instance(struct _Entities_LL_ * Entity_ptr, 
struct LD_Instance_Struct * VRAM_Instace)
{
    Entity_ptr->Entity.VRAM_Instace = VRAM_Instace;
}

void Entity_set_Model_Instance_Scale(struct _Entities_LL_* Entity_ptr,float Scale)
{
    Entity_ptr->Entity.Instance_Scale = Scale;
}

void Entity_set_Sensor_Parameters(struct _Entities_LL_*Entity_ptr,float Parameter_1,float Parameter_2)
{
    Sensor_Set_Parameters(&Entity_ptr->Entity.Sensor.Sensor,Parameter_1,Parameter_2);
}

void Entity_Update(struct _Entities_LL_*Entity_ptr)
{
    float Delta_Time = CHRONO_STRUCT.DELTA_TIME.Delta_Time;

    struct __Movement_ * M = &Entity_ptr->Entity.Movement;
    
    M->Direction_Degree+=M->Turn_Speed * M->Turn * Delta_Time;



    float DummyV2[]={M->Mov_Speed * M->Forward * Delta_Time,0};
    V2Rotate_FPU(DummyV2,&M->Direction_Degree,M->HitBox->AABB.Direction);
    

    struct LD_Instance_Struct * Instance = Entity_ptr->Entity.VRAM_Instace;
    
    if(Instance != NULL)
    {
	memcpy(Instance->Model.Matrix,Identity_Matrix_4x4,4*16);
	float Scale = Entity_ptr->Entity.Instance_Scale;
	
	if(Scale != 1.0)
	{
	    Instance->Model.Matrix[0] *=Scale;
	    Instance->Model.Matrix[5] *=Scale;
	    Instance->Model.Matrix[10] *=Scale;
	}

	float Q[4]; float QM[16]; float Ax [3] ={0,0,1.f};
	AxisAngleToQuaternion(Ax,(M->Direction_Degree+90),Q);
	QuaternionToMatrix4x4(Q,QM);
	M4x4MUL(Instance->Model.Matrix,QM,Instance->Model.Matrix);

    }
}

void Entity_pos_Update_All()
{
    _Entities_LL_*Entity = Entities_Sys.Entities_LL_First;

    while(Entity!=NULL)
    {
	struct LD_Instance_Struct *  Instance = Entity->Entity.VRAM_Instace;
	if(Instance != NULL)
        {
            memcpy(&Instance->Model.Matrix[12],Entity->Entity.Movement.HitBox->AABB.Center_Position,8);
        }
	
	Entity = Entity->Next;
    }
}

void Entity_Movement(struct _Entities_LL_*Entity_ptr,float Forward_Multiplier, float Turn_Multiplier)
{
    struct __Movement_ * M = &Entity_ptr->Entity.Movement;
    M->Forward = Forward_Multiplier;
    M->Turn = Turn_Multiplier;

    Entity_Update(Entity_ptr);
}

//To avoid Bad LightUserData problems: 
//push the same number of parameters the function uses
void Entity_AI_Script_Execute(struct _AI_LL_*AI,void * Overrider_Pointer)
{
    lua_State * Lua_Script = AI->Lua;
    if(Lua_Script != NULL)
    {
	if(Overrider_Pointer==NULL)
	{Overrider_Pointer = AI->Parameter_Pointer;}

	lua_getglobal(Lua_Script,AI->Function);
	lua_pushlightuserdata(Lua_Script,AI->Caller);
	//lua_pushlightuserdata(Lua_Script,Overrider_Pointer);
	lua_pcall(Lua_Script,1,0,0);
    }
}
#endif
