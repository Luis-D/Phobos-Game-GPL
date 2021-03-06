#include <stdio.h>
#include <stdlib.h>

#include "libLDCC/Time/LD_Time.h"
#include "libLDCC/LD_Read.h"
#include "libLDCC/IQM/LD_IQM.h"

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
#include "libLDCC/GL/OES2/LD_3D_OES.h"

#include "Collision.h"
#include "AI.h"
#include "lua_h.h"

//-Wl,-subsystem,windows
int main(void)
{
    int MAX_FPS = 60;
    float Aspect;

    glfwInit();
    GLFW_Context_Struct Context_State = Context_State_Create (640,480,0,60,0,"Tech Demo. Luis Delgado. December,19, 2018.",4);
    GLFWwindow* window = GLFW_Create_Window(&Context_State);

    Aspect = (Context_State.Width*1.f)/(Context_State.Height*1.f);

    LD_GLFW_Keyboard_Init(window);

    GL_Extensions_Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LD_3D_init();

    extern char _binary_Shaders_DeferredLighting_VS_Gbuffer_glsl_start;
    extern char _binary_Shaders_DeferredLighting_FS_Gbuffer_glsl_start;

    GLuint Shader_DeferredLight_Gbuffer =
    GL_Shaders_Load_Strings(&_binary_Shaders_DeferredLighting_VS_Gbuffer_glsl_start,
    &_binary_Shaders_DeferredLighting_FS_Gbuffer_glsl_start);

    int Uniform_Locs_Mat4[]=
    {glGetUniformLocation(Shader_DeferredLight_Gbuffer,"Matrix")};
    Pho_Camera_init();
    
    ProjectionMatrices_init(60,Aspect,
    1.f,1000.f);
   
    lua_State * L = Lua_State_Init();
    Lua_add_registers(L);
    Lua_Execute(L,"script.lua");

    printf("Script done.\n");
 
    //IQM_RAW_Struct  IQM_Anim;

    //IQM_Load_File(&IQM_Anim,"body_Walk.iqm");

   //LD_Animation_Object_Struct * Ani = LD_3D_AnimationsLinkedList_Append(&IQM_Anim.PosesCount);  
//LD_Animation_Object_Struct * Ani = LD_3D.Animations_LL->Last->Data;
//printf("Ani: ->%p\n",Ani);
   // printf("PC:%d | %d\n",Ani->FramesCount,IQM_Anim.FramesCount);

    //LD_3D_Instance_Set_Animations_Array(LD_3D.InstancesBuffer+0,1);
    //LD_3D_Instance_Set_Animation(LD_3D.InstancesBuffer+0,0,Ani,0,1.f);
    //LD_3D.InstacesBuffer[0].Animation.CurrentFrame=0;
    //*LD_3D_Instance_Animation_Flag(LD_3D.InstancesBuffer+0,0) |= 2;
    
/*
    Path_2D_struct * Path = Navi_Map_2D_FindPath(&Pho_Scene.NaviMap.Node_Array[0],&Pho_Scene.NaviMap.Node_Array[1],0);
    
    //printf("-->%x\n",Path);
    _NavNode_2D_LL * explorer = Path->First;
    while(explorer != NULL)
    {
	printf("->%lx\n",explorer->Node);
	explorer=explorer->Next;
    }
    Path_2D_Destroy(Path);
*/
/*    
explorer = Path->First;
    while(explorer != NULL)
    {
	printf("->%lx\n",explorer->Node);
	explorer=explorer->Next;Iexp->Animation.CurrentFrame+=
                    (Iexp->Animation.Data->Animations[Iexp->Animation.CurrentAnimation].framerate
                    /BASE_FPS * Delta)
                    *Iexp->Animation.Speed;
    }
  */
    struct Chara * player = &Entities_Sys.Entities_LL_First->Entity;
/*

    int NN = 0;
     struct __Movement_ * Mov = &player->Movement;
       Mov->HitBox->AABB.Center_Position[0] = Pho_Scene.NaviMap.Node_Array[NN].x;
       Mov->HitBox->AABB.Center_Position[1] = Pho_Scene.NaviMap.Node_Array[NN].y;
       //Mov->HitBox->AABB.Center_Position[0] = -3;
       //Mov->HitBox->AABB.Center_Position[1] = 3; 
	Mov->Direction_Degree = lua_tonumber(L,4);
*/


    float * Delta_Time = Delta_time_init(MAX_FPS);
    printf("Loop\n");
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    while(!glfwWindowShouldClose(window))
    {
        Delta_time_Frame_Start();

/*
	    float F=0,T=0;
        //*LD_3D_Instance_Animation_Flag(LD_3D.InstancesBuffer+0,0) &=~1;
        if(glfwGetKey(window, GLFW_KEY_LEFT))
        {T=1.f;}
        if(glfwGetKey(window, GLFW_KEY_RIGHT))
        {T=-1.f;}
        if(glfwGetKey(window, GLFW_KEY_UP))
        {F=1.f;}
        if(glfwGetKey(window, GLFW_KEY_DOWN))
        {F=-1.f;}
        //printf("F:%f | ",F);
        if(F!=0 || T!=0)
        {
            char AFLAG = *LD_3D_Instance_Animation_Flag(LD_3D.InstancesBuffer+0,0);
            if(F<0){AFLAG|=4;}
            else
            {
                AFLAG&=~4;
            }

            *LD_3D_Instance_Animation_Flag(LD_3D.InstancesBuffer+0,0)=AFLAG;   
        }
	    Entity_Movement((struct _Entities_LL_*)player,F,T);
*/
        Characters_AI_Update();
        
        Collisions_Update();

        Entity_pos_Update_All(); //<- This updates the Entities after the computations

        Pho_Camera_Update(player->Movement.HitBox->AABB.Center_Position[0],player->Movement.HitBox->AABB.Center_Position[1]);

        LD_3D_Update(*Delta_Time,BASEFPS);

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



