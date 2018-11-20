/* * * * * 
 * This Header defines the camera system for the game.
 * * * * * /
/*  - Luis Delgado. 2018 */

#ifndef _PHO_VIEW_H_
#define _PHO_VIEW_H_

#include "libLDCC/LD_Math.h"
#include <math.h>

/*
    Notes: 
    float DistanceV2(float * A, float * B) needs ASM implementation
    float DegreesV2(float * Point2D)    needs ASM implementation
    float DegreesV2V2(float * A, float * B) needs ASM implementation
*/


float DistanceV2(float * A, float * B)
{
    float XMM0[2]; float XMM1[2]; 
    memcpy(XMM0,A,8);memcpy(XMM1,B,8);
    XMM1[0]-=XMM0[0];XMM1[1]-=XMM0[1];

    return sqrtf((XMM1[0]*XMM1[0])+(XMM1[1]*XMM1[1]));
}

float DegreesV2(float * Point2D)
{
    return atanf(Point2D[1]/Point2D[0]) * 180.f / M_PI;
}

float DegreesV2V2(float * A, float * B)
{
    float XMM0[2]; float XMM1[2]; 
    memcpy(XMM0,A,8);memcpy(XMM1,B,8);
    XMM1[0]-=XMM0[0];XMM1[1]-=XMM0[1];

    return DegreesV2(XMM1);
}


/*** Global Matrices ***/
float ProjectionMatrix[32]; //First half is Perspective, second half is Orthogonal
float ViewMatrix[32];       //First half is for Perspective, second half is for Orthogonal

 // Projection * View
float ProjectionViewMatrix[32]; //First half is for Perspective, second half is for Orthogonal
/***********************/

void ProjectionMatrices_init(float FOV,float ASPECT, float NEAR, float FAR)
{
    OrthogonalProjectionMatrix4x4(ProjectionMatrix+16,ASPECT,1.f,NEAR,FAR); 
    PerspectiveProjectionMatrix4x4(ProjectionMatrix,FOV,ASPECT,NEAR,FAR);
/*
    for(int i =0;i<16;i+=4)
    {
    printf("%f, %f, %f, %f \n",(ProjectionMatrix+16)[0+i],(ProjectionMatrix+16)[1+i],(ProjectionMatrix+16)[2+i],(ProjectionMatrix+16)[3+i]);
*/	}
}

void ProjectionViewMatrix_Ortho_Update()
{
    M4x4MUL(ProjectionMatrix+16,ViewMatrix+16,ProjectionViewMatrix+16);
}

struct Pho_ViewLook
{
    struct __PC_vec_3D__
    {float x,y,z;}       From;
    struct __PC_vec_3D__ To;
    struct __PC_vec_3D__ Up;
};

#define PoolCapacity 10
struct Pho_Camera_System
{
    int CurrentPoolCapacity;
    int Cameras_Count;
   
    struct Pho_Camera
    {
        float FieldOfView; //In Degrees
        float DistanceOfView;  //how far the camera can "see"
        float DegreeAngle; //The angle of the cam
        struct Pho_ViewLook view;
    } * Cameras_Array;
    struct Pho_Camera * Current_Camera;

} Camera_System;
typedef struct Pho_Camera Pho_Camera;

void Pho_Camera_init()
{
    Camera_System.CurrentPoolCapacity = PoolCapacity;
    Camera_System.Cameras_Count=0;
    Camera_System.Cameras_Array = (struct Pho_Camera *)
    malloc(sizeof(struct Pho_Camera) * PoolCapacity);
    Camera_System.Current_Camera=NULL;
}



Pho_Camera * Pho_Camera_add(float From_X,float From_Y,float From_Z,
float To_X,float To_Y,float To_Z,float FieldOfView,float DistanceOfView)
{
    int tmp = Camera_System.Cameras_Count+1;
    if(tmp > Camera_System.CurrentPoolCapacity)
    {
        Pho_Camera * tmpptr = (Pho_Camera*)
        realloc(Camera_System.Cameras_Array,sizeof(Pho_Camera)*tmp);
        if(tmpptr == NULL){printf("ERROR::MEM\n");return NULL;}
        Camera_System.Cameras_Array = tmpptr;

        Camera_System.CurrentPoolCapacity+=PoolCapacity;
    }
    Camera_System.Cameras_Count=tmp;

    Pho_Camera * tmpcam = &Camera_System.Cameras_Array[Camera_System.Cameras_Count-1];

    tmpcam->FieldOfView=FieldOfView;
    tmpcam->DistanceOfView=DistanceOfView;
 
    tmpcam->view.From.x = From_X;
    tmpcam->view.From.y = From_Y;
    tmpcam->view.From.z = From_Z;
    tmpcam->view.To.x = To_X;
    tmpcam->view.To.y = To_Y;
    tmpcam->view.To.z = To_Z;
    tmpcam->view.Up.x = 0.f;
    tmpcam->view.Up.y = 0.f;
    tmpcam->view.Up.z = 1.f;

    tmpcam->DegreeAngle= DegreesV2V2(&tmpcam->view.From.x,&tmpcam->view.To.x);

    return tmpcam;
}

void Pho_Camera_Reset()
{
    free(Camera_System.Cameras_Array);
    Pho_Camera_init();
}

void Pho_Camera_Update(float x, float y)
{
    float Pos[2]; Pos[0] = x; Pos[1] = y;

    if(Camera_System.Current_Camera!=NULL)
    {
        struct Pho_Camera * First_Cam = Camera_System.Current_Camera;
            char In_Distance = 
            (DistanceV2(Pos,&First_Cam->view.From.x) <= First_Cam->DistanceOfView);

            float Ang = DegreesV2V2(&First_Cam->view.From.x,Pos);
            char In_FOV =  
            (180.f - abs(abs(First_Cam->DegreeAngle - Ang) - 180.f) <= (First_Cam->FieldOfView*0.5));
            if(!In_Distance || !In_FOV)
            {
                Camera_System.Current_Camera=NULL;
            }
    }

    if(Camera_System.Current_Camera==NULL)
    {
        struct Pho_Camera * First_Cam = Camera_System.Cameras_Array;
        struct Pho_Camera * PostLast_Cam = First_Cam+Camera_System.Cameras_Count;

        for (;First_Cam<PostLast_Cam;First_Cam++)
        {
            char In_Distance = 
            (DistanceV2(Pos,&First_Cam->view.From.x) <= First_Cam->DistanceOfView);

            float Ang = DegreesV2V2(&First_Cam->view.From.x,Pos);
            char In_FOV =  
            (180.f - abs(abs(First_Cam->DegreeAngle - Ang) - 180.f) <= (First_Cam->FieldOfView*0.5));

            if(In_Distance && In_FOV)
            {
                Camera_System.Current_Camera = First_Cam;
                ViewLookAt(ViewMatrix,
                (float*)&First_Cam->view.From,
                (float*)&First_Cam->view.To,
                (float*)&First_Cam->view.Up);
                M4x4MUL(ProjectionMatrix,ViewMatrix,ProjectionViewMatrix);
                break;
            }
        }
    }
}

#endif
