#include <stdio.h>
#include <stdlib.h>

#include "libLDCC/LD_C_Chrono.h"
#include "libLDCC/LD_Read.h"
#include "libLDCC/LD_IQM.h"
#include "libLDCC/LD_Polygon_Extract_3D.h"
#include "libLDCC/LD_STL.h"
#include "libLDCC/LD_Navi_2D.h"

#ifdef _WIN2
    #include <windows.h>
#endif

#if defined(__linux__) || defined(__FORCE_OES2)
#include "libLDCC/GL/OES2/LD_OES2_GLFW3.h"
#else
#include "libLDCC/GL/DESKTOP/LD_OES2_FALLBACK_GLFW3.h" 
#include "libLDCC/GL/DESKTOP/GLFW_GL_EXT.h"
#endif

#include "libLDCC/GL/OES2/OES_DDS.h"
#include "libLDCC/GL/GL_SHADERS.h"
#include "libLDCC/GL/OES2/LD_Def_Lighting_OES.h"

#include "Characters.h"

//-Wl,-subsystem,windows
int main(void)
{
    int MAX_FPS = 60;

    glfwInit();
    GLFW_Context_Struct Context_State = Context_State_Create (640,480,0,60,0,"Tech Demo. Luis Delgado. October 4th, 2018.",4);
    GLFWwindow* window = GLFW_Create_Window(&Context_State);

    printf("ok\n");

    GL_Extensions_Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LD_3D_init();

    GLuint Shader_DeferredLight_Gbuffer =
    GL_Shaders_Load_File("Shaders/DeferredLighting/VS_Gbuffer.glsl","Shaders/DeferredLighting/FS_Gbuffer.glsl");

    int Uniform_Locs_Mat4[]=
    {glGetUniformLocation(Shader_DeferredLight_Gbuffer,"Matrix")};



    IQM_RAW_Struct _Modelo_;
    if(IQM_Load_File(&_Modelo_,"mono.iqm") ==-1){printf("no\n");}
    LD_Model_Object_Struct * Modelo = LD_3D_ModelsLinkedList_Append((&_Modelo_.VertexCount));
     
    IQM_RAW_Struct _Modelo_2;
    if(IQM_Load_File(&_Modelo_2,"test1.iqm") ==-1){printf("no\n");}
    LD_Model_Object_Struct * Modelo2 = LD_3D_ModelsLinkedList_Append((&_Modelo_2.VertexCount));

    LD_3D_InstaceBuffer_Set_capacity(2);
    LD_3D_VRAMBuffer_allocate(2);

    LD_3D_Fill_Instance(&LD_3D.InstacesBuffer[0],Modelo);
    LD_3D_Fill_Instance(&LD_3D.InstacesBuffer[1],Modelo2);

    LD_3D_Fill_VRAMBuffer(LD_3D.VRAMBuffer,&LD_3D.InstacesBuffer[0],&LD_3D.InstacesBuffer[0],0);
    LD_3D_Fill_VRAMBuffer(LD_3D.VRAMBuffer+1,&LD_3D.InstacesBuffer[1],&LD_3D.InstacesBuffer[1],0);

    STL_Mesh_NoNormals_Struct NMESH;

    int o =STL_BIN_Load_NoNormals("test1.stl",&NMESH);
   
    Edges_Map_2D_struct EdgesMap;
 
    EdgesMap.EdgesBuffer = (Line_Segment_2D_Struct*)  Triangles_3D_Extract_Border_Edges_to_2D_no_Z(
    (float*) NMESH.Triangle_Array,NMESH.Triangles_Count,&EdgesMap.Edges_Count);


    float ProjectionMatrix [16];
    memcpy(ProjectionMatrix,Identity_Matrix_4x4,16*4);
    PerspectiveProjectionMatrix4x4(ProjectionMatrix,45,Context_State.Width*1.f/Context_State.Height*1.f,1,1000.f);
    
    float ViewMatrix [16]; float VPMatrix [16];

    printf("Loop\n");
    float * Delta_Time = Delta_time_init(MAX_FPS);
    
    Chara player;
    player.Movement.Forward=0.f;
    player.Movement.Turn =0.f;
    player.Movement.Angle_Speed = 10.f;
    player.Movement.Mov_Speed = 0.2;
    player.Movement.Degree_Angle = 0.f;
    player.Movement.AABB.Center_Position[0]=0;
    player.Movement.AABB.Center_Position[1]=-3;
    player.Movement.AABB.Half_Extent[0]=0.2f;
    player.Movement.AABB.Half_Extent[1]=0.2f;
    player.Instace = &LD_3D.InstacesBuffer[0];


    while(!glfwWindowShouldClose(window))
    {
        Delta_time_Frame_Start();

        player.Movement.Forward=0.f;
        player.Movement.Turn =0.f;
        if(glfwGetKey(window, GLFW_KEY_LEFT))
        {player.Movement.Turn=1.f;}
        if(glfwGetKey(window, GLFW_KEY_RIGHT))
        {player.Movement.Turn=-1.f;}

        if(glfwGetKey(window, GLFW_KEY_UP))
        {player.Movement.Forward=1.f;}
        if(glfwGetKey(window, GLFW_KEY_DOWN))
        {player.Movement.Forward=-1.f;}

        player.Movement.Degree_Angle+=
        player.Movement.Angle_Speed * player.Movement.Turn * *Delta_Time;
        memcpy(player.Instace->Matrix,Identity_Matrix_4x4,4*16);
        player.Instace->Matrix[0] *=0.2;
        player.Instace->Matrix[5] *=0.2;
        player.Instace->Matrix[10] *=0.2;
        float Q[4]; float QM[16]; float Ax [3] ={0,0,1.f};
        AxisAngleToQuaternion(Ax,(player.Movement.Degree_Angle-90)*-1.f,Q);
        QuaternionToMatrix4x4(Q,QM);
        M4x4MUL(player.Instace->Matrix,QM,player.Instace->Matrix);
        player.Movement.AABB.Direction[0]=0;
        player.Movement.AABB.Direction[1]=0;
        float DummyV2[2] = {player.Movement.Mov_Speed * player.Movement.Forward * *Delta_Time,0};
        V2Rotate_FPU(DummyV2,&player.Movement.Degree_Angle,player.Movement.AABB.Direction);       

int col = Edges_Map_2D_vs_Swept(&EdgesMap, &player.Movement.AABB, Swept_AABB_vs_Line_Segment_2D_Check,Swept_AABB_2D_Response_Slide,-1.f);
if(!col){ V2V2ADD(player.Movement.AABB.Center_Position,player.Movement.AABB.Direction,player.Movement.AABB.Center_Position);}

V2V2ADD(&player.Instace->Matrix[12],player.Movement.AABB.Center_Position,&player.Instace->Matrix[12]);
player.Instace->Matrix[14]=0.2;      
 
float Cam [9] = {8,-8,8,0,0,0,0,0,1};
   if(player.Movement.AABB.Center_Position[0] > 5)
    {
    float camm[9] = {-4,-2,6, 8,2,0, 0,0,1}; memcpy(Cam,camm,9*4);
} 
     ViewLookAt(ViewMatrix,Cam,Cam+3,Cam+6);
    
	LD_3D_Update();

        M4x4MUL(ProjectionMatrix,ViewMatrix,VPMatrix);

        glClearColor(1.f,0.f,1.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glUseProgram(Shader_DeferredLight_Gbuffer);
        glUniformMatrix4fv(Uniform_Locs_Mat4[0],1,0,VPMatrix);  
        glDisable(GL_BLEND);
        LD_3D_Draw_G_Pass();
        glEnable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();       

        Delta_time_Frame_End();
    }
 
    glfwTerminate();

    return 1;
}


