#ifndef __Characters_H_
#define __Characters_H_

#include "libLDCC/LD_Collision_2D.h"

struct Entities_Sys_Struct
{
    int Entities_Count;

    struct _AABB_LL_
    {
        Swept_AABB_2D_Struct AABB;
        struct _AABB_LL_ * Next;
        struct _AABB_LL_ * Last;
    } * AABB_LL_First;
    struct _AABB_LL_ * AABB_LL_Last;

 

    struct _Entities_LL_
    {
        struct Chara 
        {
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

void Entities_Sys_init()
{
    Entities_Sys.Entities_Count=0;
    Entities_Sys.Entities_LL_First=NULL;
    Entities_Sys.Entities_LL_Last=NULL;
   
    Entities_Sys.AABB_LL_First=NULL;
    Entities_Sys.AABB_LL_Last=NULL;
}


_Entities_LL_ * Entity_Create(float Pos_X, float Pos_Y,float Direction_Degree,
 float Movement_Speed, float Turn_Speed,float HitBox_Half_Length)
{
    if(Entities_Sys.Entities_LL_Last == NULL)
    {
        Entities_Sys.Entities_LL_First = (struct _Entities_LL_*) 
        malloc(sizeof(struct _Entities_LL_));
        Entities_Sys.AABB_LL_First = (struct _AABB_LL_*) 
        malloc(sizeof(struct _AABB_LL_));

        if(Entities_Sys.Entities_LL_First==NULL || 
        Entities_Sys.AABB_LL_First==NULL){return NULL;}

        Entities_Sys.Entities_LL_First->Last=NULL;
        Entities_Sys.Entities_LL_Last = Entities_Sys.Entities_LL_First;

        Entities_Sys.AABB_LL_First->Last=NULL;
        Entities_Sys.AABB_LL_Last = Entities_Sys.AABB_LL_First;
    }
    else
    {
        Entities_Sys.Entities_LL_Last->Next = (struct _Entities_LL_*) 
        malloc(sizeof(struct _Entities_LL_));

        Entities_Sys.AABB_LL_Last->Next = (struct _AABB_LL_*) 
        malloc(sizeof(struct _AABB_LL_));

        if(Entities_Sys.Entities_LL_Last==NULL || 
        Entities_Sys.AABB_LL_Last){return NULL;}


        struct _Entities_LL_ * tmp2 = Entities_Sys.Entities_LL_Last->Next;
        tmp2->Last=Entities_Sys.Entities_LL_Last;
        Entities_Sys.Entities_LL_Last = tmp2;

        struct _AABB_LL_ * tmp3 = Entities_Sys.AABB_LL_Last->Next;
        tmp3->Last=Entities_Sys.AABB_LL_Last;
        Entities_Sys.AABB_LL_Last = tmp3;
    }

    Entities_Sys.Entities_LL_Last->Next=NULL;
    Entities_Sys.Entities_LL_Last->Entity.Movement.HitBox = Entities_Sys.AABB_LL_Last;

    struct Chara * tmp = &Entities_Sys.Entities_LL_Last->Entity;
    tmp->Movement.Turn_Speed = Turn_Speed;
    tmp->Movement.Direction_Degree = Direction_Degree;
    tmp->Movement.Mov_Speed = Movement_Speed;
    tmp->Movement.HitBox->AABB.Center_Position[0] = Pos_X;
    tmp->Movement.HitBox->AABB.Center_Position[1] = Pos_Y;
    tmp->Movement.HitBox->AABB.Half_Extent[0] = HitBox_Half_Length;
    tmp->Movement.HitBox->AABB.Half_Extent[1] = HitBox_Half_Length;
    tmp->Movement.z=0;

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

void Entity_Delete(struct _Entities_LL_ * Entity_ptr)
{
    struct _Entities_LL_ * N;
    struct _Entities_LL_ * L;

    N = Entity_ptr->Next;
    L = Entity_ptr->Last;

    L->Next=N;
    N->Last=L;

    _HitBox_Delete(Entity_ptr->Entity.Movement.HitBox);

    free(Entity_ptr);

    Entities_Sys.Entities_Count--;
}

void Entity_set_Model_Instance(struct _Entities_LL_ * Entity_ptr, 
struct LD_Instance_Struct * VRAM_Instace)
{
    Entity_ptr->Entity.VRAM_Instace = VRAM_Instace;
}

#endif