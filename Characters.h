#ifndef __Characters_H_
#define __Characters_H_

#include "libLDCC/LD_Collision_2D.h"

typedef struct __Point3D_
{
    float x,y,z;
} __Point3D_;

typedef struct Chara 
{
    struct LD_Instance_Struct * Instace;
    struct __Movement_
    {   float Forward, Turn; 
        float Angle_Speed;
        float Degree_Angle;
        float Mov_Speed;
        Swept_AABB_2D_Struct AABB;
        float z;
    } Movement;
}Chara;

#endif