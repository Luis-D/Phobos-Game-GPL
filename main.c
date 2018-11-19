#include <stdio.h>
#include <stdlib.h>

#include "libLDCC/LD_Time.h"
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

#include "View.h"

#include "libLDCC/GL/OES2/OES_DDS.h"
#include "libLDCC/GL/GL_SHADERS.h"
#include "libLDCC/GL/OES2/LD_Def_Lighting_OES.h"

#include "Characters.h"

#include "lua_h.h"

//-Wl,-subsystem,windows
int main(void)
{


    int MAX_FPS = 60;
    float Aspect;

    glfwInit();
    GLFW_Context_Struct Context_State = Context_State_Create (640,480,0,60,0,"Tech Demo. Luis Delgado. October 4th, 2018.",4);
    GLFWwindow* window = GLFW_Create_Window(&Context_State);

    Aspect = (Context_State.Width*1.f)/(Context_State.Height*1.f);

    GL_Extensions_Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LD_3D_init();

    GLuint Shader_DeferredLight_Gbuffer =
    GL_Shaders_Load_File("Shaders/DeferredLighting/VS_Gbuffer.glsl","Shaders/DeferredLighting/FS_Gbuffer.glsl");

    GLuint Shader_ShadowVol =
    GL_Shaders_Load_File("Shaders/DeferredLighting/VS_ShadowVol.glsl","Shaders/DeferredLighting/FS_Gbuffer.glsl");

    int Uniform_Locs_Mat4[]=
    {glGetUniformLocation(Shader_DeferredLight_Gbuffer,"Matrix")};

    IQM_RAW_Struct _Modelo_;
    if(IQM_Load_File(&_Modelo_,"mono.iqm") ==-1){printf("no\n");}
    LD_Model_Object_Struct * Modelo = LD_3D_ModelsLinkedList_Append((&_Modelo_.VertexCount));
     
    IQM_RAW_Struct _Modelo_2;
    if(IQM_Load_File(&_Modelo_2,"test1.iqm") ==-1){printf("no\n");}
    LD_Model_Object_Struct * Modelo2 = LD_3D_ModelsLinkedList_Append((&_Modelo_2.VertexCount));

    LD_3D_InstaceBuffer_Set_capacity(3);
    LD_3D_VRAMBuffer_allocate(2);

    LD_3D_Fill_Instance(&LD_3D.InstacesBuffer[0],Modelo);
    LD_3D_Fill_Instance(&LD_3D.InstacesBuffer[1],Modelo2);

    LD_3D_Fill_VRAMBuffer(LD_3D.VRAMBuffer,&LD_3D.InstacesBuffer[0],&LD_3D.InstacesBuffer[0],0);
    LD_3D_Fill_VRAMBuffer(LD_3D.VRAMBuffer+1,&LD_3D.InstacesBuffer[1],&LD_3D.InstacesBuffer[1],0);

    Pho_Camera_init();
    
    ProjectionMatrices_init(60,Aspect,
    1.f,1000.f);
   
    Pho_Camera_add(0,-2,10,0,0,0,50,5);
    Pho_Camera_add(8,-5,3,4,0,0,90,4);
    Pho_Camera_add(8,-5,3,12,0,0,90,4);


    _Entities_LL_ * tmpplayer = Entity_Create(0,0,0,0.30,10,0.25);
    Entity_set_Model_Instance(tmpplayer,&LD_3D.InstacesBuffer[0]);

    struct Chara * player = &Entities_Sys.Entities_LL_First->Entity;

     STL_Mesh_NoNormals_Struct NMESH;
    int o =STL_BIN_Load_NoNormals("test1.stl",&NMESH);
    Edges_Map_2D_struct EdgesMap;
    EdgesMap.EdgesBuffer = (Line_Segment_2D_Struct*)  Triangles_3D_Extract_Border_Edges_to_2D_no_Z(
    (float*) NMESH.Triangle_Array,NMESH.Triangles_Count,&EdgesMap.Edges_Count);

    float * Delta_Time = LD_Timer_init(10,MAX_FPS);
    printf("Loop\n");
    while(!glfwWindowShouldClose(window))
    {
        Delta_time_Frame_Start();


        player->Movement.Forward=0.f;
        player->Movement.Turn =0.f;
        if(glfwGetKey(window, GLFW_KEY_LEFT))
        {player->Movement.Turn=1.f;}
        if(glfwGetKey(window, GLFW_KEY_RIGHT))
        {player->Movement.Turn=-1.f;}

        if(glfwGetKey(window, GLFW_KEY_UP))
        {player->Movement.Forward=1.f;}
        if(glfwGetKey(window, GLFW_KEY_DOWN))
        {player->Movement.Forward=-1.f;}



        player->Movement.Direction_Degree+=
        player->Movement.Turn_Speed * player->Movement.Turn * *Delta_Time;


        memcpy(player->VRAM_Instace->Matrix,Identity_Matrix_4x4,4*16);


        player->VRAM_Instace->Matrix[0] *=0.2;
        player->VRAM_Instace->Matrix[5] *=0.2;
        player->VRAM_Instace->Matrix[10] *=0.2;


        float Q[4]; float QM[16]; float Ax [3] ={0,0,1.f};
        AxisAngleToQuaternion(Ax,(player->Movement.Direction_Degree-90)*-1.f,Q);
        QuaternionToMatrix4x4(Q,QM);
        
        M4x4MUL(player->VRAM_Instace->Matrix,QM,player->VRAM_Instace->Matrix);
        player->Movement.HitBox->AABB.Direction[0]=0;
        player->Movement.HitBox->AABB.Direction[1]=0;
    
        float DummyV2[2] = {player->Movement.Mov_Speed * player->Movement.Forward * *Delta_Time,0};
        V2Rotate_FPU(DummyV2,&player->Movement.Direction_Degree,player->Movement.HitBox->AABB.Direction);       


        int col = Edges_Map_2D_vs_Swept(&EdgesMap, &player->Movement.HitBox->AABB, Swept_AABB_vs_Line_Segment_2D_Check,Swept_AABB_2D_Response_Slide,-1.f);
        
        if(!col){ V2V2ADD(player->Movement.HitBox->AABB.Center_Position,player->Movement.HitBox->AABB.Direction,player->Movement.HitBox->AABB.Center_Position);}

        V2V2ADD(&player->VRAM_Instace->Matrix[12],player->Movement.HitBox->AABB.Center_Position,&player->VRAM_Instace->Matrix[12]);
        player->VRAM_Instace->Matrix[14]=0.2;      
 
        Pho_Camera_Update(player->Movement.HitBox->AABB.Center_Position[0],player->Movement.HitBox->AABB.Center_Position[1]);

/*
        printf("cam->%x | pos: %f,%f\n",Camera_System.Current_Camera,
        player->Movement.HitBox->AABB.Center_Position[0],player->Movement.HitBox->AABB.Center_Position[1]
        );
*/

	    LD_3D_Update();
        glClearColor(1.f,0.f,1.f,1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glUseProgram(Shader_DeferredLight_Gbuffer);
        glUniformMatrix4fv(Uniform_Locs_Mat4[0],1,0,ProjectionViewMatrix);  
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


