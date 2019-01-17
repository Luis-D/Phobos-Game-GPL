#ifndef PHO_LUA_L
#define PHO_LUA_L

#include <luajit-2.1/luajit.h>
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>

#include "Scene.h"

#include "libLDCC/Time/LD_Time.h"
#include "libLDCC/Extern/LD_Math.h"
#include "libLDCC/Input/GLFW3/LD_Keyboard_GLFW.h"
#include "Lua_Utils.h"
#include "Lua_LD_Math.h"

int Angles_Check_Side(float Angle, float Test) //Needs ASM implementation
{
    float Diff =  Test - Angle;

    if(Diff < -180.0f){Diff += 360.f;}
    if(Diff > 180.0f){Diff -= 360.f;}

    if(Diff > 0.0f){return 1;}
    if(Diff < 0.0f){return -1;}

    return 0;
}

float Angle_Diff(float Angle, float Test) //Needs ASM implementation
{
    return 180.f - fabsf(fabsf(Angle - Test)-180.f);
}

float Angle_0_360(float Angle) //Needs ASM implementation
{
    return Angle-((floor(Angle/360.f))*360.f);
}




void bail(lua_State *L, char *msg){
	fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
		msg, lua_tostring(L, -1));
	exit(1);
}

lua_State * Lua_Thread(lua_State **L)
{
    *L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(*L);                           /* Load Lua libraries */
    lua_State* thread = lua_newthread(*L);
    return thread;
}

lua_State * Lua_State_Init()
{
	lua_State * L;
	L = luaL_newstate();
	luaL_openlibs(L);
	return L;
}

int Lua_Execute(lua_State * Lua_State,const char * Script_file_path)
{
    if (luaL_loadfile(Lua_State, Script_file_path))    /* Load but don't run the Lua script */
    {bail(Lua_State, "luaL_loadfile() failed");}      /* Error out if file can't be read */

                 /* Run the loaded Lua script */

    return lua_resume(Lua_State, 0);
}

void Lua_Close(lua_State * L){lua_close(L);}




void Lua_add_registers(lua_State *L); //<- Only declarationW
#define LDEC(Function_Name) int l_##Function_Name (lua_State * L)
#define LADD(Function_Name) lua_register(L,#Function_Name,l_##Function_Name );
#define LADD_EXT(Function_Name,Lua_Name) lua_register(L,Lua_Name,l_##Function_Name );

LDEC(Camera_add)
{
    //printf("camera added\n");
    Pho_Camera_add(lua_tonumber(L,1),lua_tonumber(L,2),lua_tonumber(L,3),
    lua_tonumber(L,4),lua_tonumber(L,5),lua_tonumber(L,6),lua_tonumber(L,7),
    lua_tonumber(L,8));
    return 0;
}

LDEC(Model_add)
{
  	IQM_RAW_Struct *  _Model_ = (IQM_RAW_Struct *) malloc(sizeof(IQM_RAW_Struct));
    if(IQM_Load_File(_Model_,lua_tostring(L,1)) ==-1){printf("ERROR::MODEL\n");}
	LD_Model_Object_Struct * Model = LD_3D_ModelsLinkedList_Append((&_Model_->VertexCount));
	free(_Model_);
	lua_pushlightuserdata(L,&LD_3D.Models_LL->Last->Data);
	return 1;  
}

LDEC(InstanceBuffer_Set_Capacity)
{
    //printf("VRAM Instances: %ld\n",lua_tointeger(L,1));
	LD_3D_InstanceBuffer_Set_capacity(lua_tointeger(L,1));
	return 0;
}

LDEC(VRAMBuffers_allocate)
{
    //printf("VRAM BUFFERS: %ld\n",lua_tointeger(L,1));
	LD_3D_VRAMBuffer_allocate(lua_tointeger(L,1));
	return 0;
}

LDEC(VRAM_Instance_Create)
{
    	//printf("Model: \n", (LD_Model_Object_Struct*)lua_topointer(L,2));
	struct LD_Model_Object_Struct*tmp = (struct LD_Model_Object_Struct*)lua_topointer(L,2);
	LD_3D_Fill_Instance(&LD_3D.InstancesBuffer[lua_tointeger(L,1)],tmp);
	lua_pushlightuserdata(L,&LD_3D.InstancesBuffer[lua_tointeger(L,1)]);
	return 1;
}

LDEC(Fill_VRAM_Buffer)
{
    LD_3D_Fill_VRAMBuffer(&LD_3D.VRAMBuffer[lua_tointeger(L,1)],&LD_3D.InstancesBuffer[lua_tointeger(L,2)],
		&LD_3D.InstancesBuffer[lua_tointeger(L,3)],lua_tointeger(L,4));
	return 0;
}

LDEC(Entity_Create)
{
    char * Function = NULL;
    if(lua_gettop(L)>8)
    {Function = (char*) lua_tostring(L,9);}

    _Entities_LL_ * RET =Entity_Create(
    lua_tonumber(L,1),
    lua_tonumber(L,2),
    lua_tonumber(L,3),
    lua_tonumber(L,4),
    lua_tonumber(L,5),
    lua_tonumber(L,6),
    lua_tonumber(L,7),
    (lua_State*)lua_topointer(L,8),
    Function);

    if(lua_gettop(L)>9)
    {
	Entity_set_Model_Instance(RET,
	(struct LD_Instance_Struct *)lua_topointer(L,10));
    }
    
    if(lua_gettop(L)>10)
    {
	Entity_set_Model_Instance_Scale(RET,lua_tonumber(L,11));
    }

    Entity_Update(RET);

    lua_pushlightuserdata(L,RET);
   return 1; 
}

LDEC(Entity_Set_VRAM_Instance)
{
    Entity_set_Model_Instance(
        (struct _Entities_LL_ *)lua_topointer(L,1),
        (struct LD_Instance_Struct *)lua_topointer(L,2));
    return 0;
}

LDEC(Entity_Set_VRAM_Scale)
{
    Entity_set_Model_Instance_Scale((_Entities_LL_*)lua_topointer(L,1),lua_tonumber(L,2));
    return 0;
}

LDEC(Scene_Set_Map)
{
    int RET = Scene_Set_Map_STLbin((char*)lua_tostring(L,1));
    lua_pushinteger(L,RET);
    return 1;
}

LDEC(Script_Destroy)
{
    lua_State * SCRIPT = (lua_State *) lua_topointer(L,1);
    lua_close(SCRIPT);
    return 0;
}

LDEC(Script_Load)
{
    lua_State * RET=luaL_newstate();
    luaL_openlibs(RET);
    int err =luaL_loadfile(RET,(char*)lua_tostring(L,1));
    
    if(err!=0)
    {
	lua_close(RET);
	RET=NULL;
    }

    Lua_add_registers(RET);
    lua_pcall(RET,0,0,0);
    lua_pushlightuserdata(L,RET);
    return 1;
}

LDEC(Script_This)
{
    lua_pushlightuserdata(L,L);
    return 1;
}

LDEC(Scene_Set_Script_File)
{
    Scene_Set_Lua_Script((char*)lua_tostring(L,1));
    Lua_add_registers(Pho_Scene.Lua_Script);
    lua_pcall(Pho_Scene.Lua_Script,0,0,0);
    return 0;
}

LDEC(Scene_Set_Script)
{
    Pho_Scene.Lua_Script = (lua_State*) lua_topointer(L,1);
    return 0;
}

LDEC(Scene_Trigger_add)
{
    float Segment[] = 
	{lua_tonumber(L,1),lua_tonumber(L,2),
	lua_tonumber(L,3),lua_tonumber(L,4)};
	
	void * OverPTR = (void *) lua_topointer(L,6);
	
	lua_pushlightuserdata(L,
	Scene_Trigger_Add(Segment,Segment+2,
	(char*)lua_tostring(L,5),OverPTR));
	return 1;
}

LDEC(Scene_Find_Node_EXT)
{
    float Point_2D[]={lua_tonumber(L,1),lua_tonumber(L,2)};
    lua_pushlightuserdata(L,Scene_Get_Node(Point_2D));
    return 1;
}

LDEC(Scene_Find_Node)
{
    lua_pushlightuserdata(L,Scene_Get_Node((float*)lua_topointer(L,1)));
    return 1;
}

LDEC(Entity_Teleport)
{
	struct __Movement_ * Mov = (struct __Movement_*)
	&(((struct _Entities_LL_*) lua_topointer(L,1))->Entity.Movement);
	Mov->HitBox->AABB.Center_Position[0] = lua_tonumber(L,2);
	Mov->HitBox->AABB.Center_Position[1] = lua_tonumber(L,3);
	Mov->Direction_Degree = lua_tonumber(L,4);
	return 0;
}

LDEC(Entity_Get_Position)
{
    float * Position =
    ((_Entities_LL_*)lua_topointer(L,1))->Entity.Movement.HitBox->AABB.Center_Position;
    lua_pushlightuserdata(L,Position);
    return 1;
}

LDEC(Entity_Get_Position_EXT)
{
    float * Position =
    ((_Entities_LL_*)lua_topointer(L,1))->Entity.Movement.HitBox->AABB.Center_Position;
    lua_pushnumber(L,Position[0]);
    lua_pushnumber(L,Position[1]);
    return 2;
}


static char __ENTITY_GOTO_(struct _Entities_LL_ * Entity,float * Point2D, float Forward,float Turn)
{
    float * Position =Entity->Entity.Movement.HitBox->AABB.Center_Position;
    float Delta = CHRONO_STRUCT.DELTA_TIME.Delta_Time;
    float DistanceToMove = Entity->Entity.Movement.Mov_Speed*Forward*Delta;
    float DistanceToPoint =V2Distance(Position,Point2D);
    if(DistanceToPoint<=DistanceToMove)
    {memcpy(Position,Point2D,8);Forward=0;return 1;}
    
	float AngleToPoint;V2DegreesV2_FPU(Position,Point2D,&AngleToPoint);
	float Angle = Entity->Entity.Movement.Direction_Degree;
	Angle = Angle_0_360(Angle);
	AngleToPoint = Angle_0_360(AngleToPoint);
	
	float AngleToTurn = Entity->Entity.Movement.Turn_Speed*Turn*Delta; 
	float AngleDiff = Angle_Diff(Angle,AngleToPoint);
	
    if( Turn>= 0)
    {

	if((AngleDiff) <= AngleToTurn)
	{
	    if(AngleToTurn != 0)
	    {Turn = AngleDiff / AngleToTurn;}
	    else{Turn=0;}
	}   
	if(Angles_Check_Side(Angle,AngleToPoint)==-1){Turn*=-1;}
//	printf("Ang: %f | Diff: %f | Turn: %f | %f\n",Angle,AngleDiff,AngleToTurn,Turn);
    }
    else
    {
	Turn=0;
	Entity->Entity.Movement.Direction_Degree=AngleToPoint;
    }
	//printf("%f | %f \n",Angle,AngleToPoint);
       Entity_Movement(Entity,Forward,Turn);
    return 0;
}

LDEC(Entity_Goto)
{
    struct _Entities_LL_ * Entity =(_Entities_LL_*)lua_topointer(L,1);
    float * Point2D=(float *)lua_topointer(L,2);
    float Forward = 1.f, Turn = -1;
    if(lua_gettop(L) > 3)
    {Forward = lua_tonumber(L,4);}
    if(lua_gettop(L) > 4)
    {Turn = lua_tonumber(L,5);}
    char RET = __ENTITY_GOTO_(Entity,Point2D,Forward,Turn); 
    lua_pushboolean(L,RET);
    return 1;
}

LDEC(Entity_Goto_EXT)
{
    struct _Entities_LL_ * Entity =(_Entities_LL_*)lua_topointer(L,1);
    float Point2D[]={lua_tonumber(L,2),lua_tonumber(L,3)};
    float Forward = 1.f, Turn = -1;
    if(lua_gettop(L) > 3)
    {Forward = lua_tonumber(L,4);}
    if(lua_gettop(L) > 4)
    {Turn = lua_tonumber(L,5);}
    char RET = __ENTITY_GOTO_(Entity,Point2D,Forward,Turn); 
    lua_pushboolean(L,RET);
    return 1;
}

LDEC(Entity_Movement)
{
    Entity_Movement((struct _Entities_LL_*)lua_topointer(L,1),
    lua_tonumber(L,2),lua_tonumber(L,3));
    return 0;
}

LDEC(Entity_Path_Set)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    struct Path_2D_struct * Path = (struct Path_2D_struct*)lua_topointer(L,2);
    Entity->Entity.AI.PathFinder.Path = Path;
    return 0;
}

LDEC(Entity_Path_Clear)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    char Erase = 1;
    if(lua_gettop(L)>1){Erase = lua_tointeger(L,2);}
    if(Erase == 1){Path_2D_Destroy(Entity->Entity.AI.PathFinder.Path);}
    Entity->Entity.AI.PathFinder.Path=NULL;
    return 0;
}

LDEC(Entity_Get_Path)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    Path_2D_struct* PATH =Entity->Entity.AI.PathFinder.Path;
    //if(PATH == NULL){printf("NULL\n");}
    if(PATH == NULL){return 0;}
    lua_pushlightuserdata(L,PATH);
    return 1;
}

LDEC(Entity_Path_Get_XY)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    float Point2D[2];
    Entity_AI_Evaluate_Path(Entity,Point2D);
    //printf("%f,%f\n",Point2D[0],Point2D[1]);
    lua_pushnumber(L,(float) Point2D[0]);
    lua_pushnumber(L,(float) Point2D[1]);
    return 2;
}

LDEC(Entity_Get_ID)
{
    lua_pushinteger(L,((struct _Entities_LL_*)lua_topointer(L,1))->Entity.ID );
    return 1;
}

LDEC(Entity_Get_Data)
{
    lua_pushlightuserdata(L,((struct _Entities_LL_*)lua_topointer(L,1))->Entity.Data);
    return 1;
}

LDEC(Entity_Set_Data)
{
    ((struct _Entities_LL_*)lua_topointer(L,1))->Entity.Data   ;
    return 1;
}

LDEC(Entity_Set_Sensor)
{
    struct _Entities_LL_*Entity = ((struct _Entities_LL_*)lua_topointer(L,1));
    Entity_set_Sensor_Parameters(Entity,lua_tonumber(L,2),lua_tonumber(L,3));
    return 0;
}

LDEC(Entity_Sensor_Check_Entity)
{
    char Bool = 0;
    struct _Entities_LL_* Caller = (struct _Entities_LL_*) lua_topointer(L,1);
    struct _Entities_LL_* Test = (struct _Entities_LL_*) lua_topointer(L,2);
    Bool=Entity_AI_Sensor_Check_Entity(Caller,Test);
    lua_pushboolean(L,Bool);
    return 1;
}

LDEC(Entity_Sensor_Update)
{
    char Boolean =Entity_AI_Sensor_Update( (struct _Entities_LL_*) lua_topointer(L,1));
    lua_pushboolean(L,Boolean);
    return 1;
}

LDEC(Path_Find)
{
    Path_2D_struct *RET=NULL;
    RET=Navi_Map_2D_FindPath((_NavNode_2D_*)lua_topointer(L,1),(_NavNode_2D_*)lua_topointer(L,2),lua_tonumber(L,3));

    int varc = lua_gettop(L);

    if(varc > 3) //Destiny has been especified//
    {
	if(varc>4)
	{
	    RET->Destiny[0] = lua_tonumber(L,4);
	    RET->Destiny[1] = lua_tonumber(L,5);
	}
	else{memcpy(RET->Destiny,(float*) lua_topointer(L,4),8);}
    }

    lua_pushlightuserdata(L,RET);
    return 1;
}

LDEC(Path_Destroy)
{
    Path_2D_Destroy((Path_2D_struct*)lua_topointer(L,1));
    return 0;
}

LDEC(VRAM_Buffer_Clear)
{
	LD_3D_VRAMBuffer_Clear(&LD_3D.VRAMBuffer[lua_tointeger(L,1)],1);
	return 0;
}

LDEC(Model_delete_all)
{
	LD_3D_ModelsLinkedList_System_Delete(NULL);
	return 0;
}

LDEC(Model_delete_Stack)
{
	LD_3D_ModelsLinkedList_System_Delete((struct LD_Model_Object_Struct *)lua_tointeger(L,1));
	return 0;
}

LDEC(Animation_add)
{
    IQM_RAW_Struct *  _Anim_ = (IQM_RAW_Struct *) malloc(sizeof(IQM_RAW_Struct));
    if(IQM_Load_File(_Anim_,lua_tostring(L,1)) ==-1){printf("ERROR::ANIMATION\n");}
    LD_Animation_Object_Struct * Ani = LD_3D_AnimationsLinkedList_Append(&_Anim_->PosesCount); 
	lua_pushlightuserdata(L,LD_3D.Animations_LL->Last->Data);
	return 1; 
}

LDEC(Entity_Get_Instance)
{
    lua_pushlightuserdata(L,((struct Chara *)lua_topointer(L,1))->VRAM_Instace);
    return 1;
}

LDEC(Instance_Animations_Set_Capacity)
{
    struct LD_Instance_Struct * Instance = 
        (struct LD_Instance_Struct*)    lua_topointer(L,1);
    unsigned int Capacity =             lua_tointeger(L,2);

    LD_3D_Instance_Set_Animations_Array(Instance,Capacity);
    return 0;
}

LDEC(Instance_Set_Animation)
{
    struct LD_Instance_Struct * Instance = 
        (struct LD_Instance_Struct*)            lua_topointer(L,1);
    unsigned int BufferIndex =                  lua_tointeger(L,2);
    struct LD_Animation_Object_Struct * Ani =
        (struct LD_Animation_Object_Struct*)     lua_topointer(L,3);
    unsigned int AnimIndex =                  lua_tointeger(L,4);

    float Speed = 1.f;

    int varc = lua_gettop(L);
    if(varc > 4) //Speed has been especified//
    {Speed = lua_tonumber(L,5);}

     LD_3D_Instance_Set_Animation(Instance,
     BufferIndex,Ani,AnimIndex,Speed);
    return 0;
}

LDEC(Instance_Animation_Set_Frame)
{
    struct LD_Instance_Struct * Instance =                           
        (struct LD_Instance_Struct*)            lua_topointer(L,1); 
    unsigned int BufferIndex =                  lua_tointeger(L,2); 
    int Frame = lua_tointeger(L,3);    
    struct _LD_Instance_Animation_Descriptor * CurrAnim = 
    (Instance->Animation.Animations_Array+BufferIndex);

    CurrAnim->CurrentFrame= (float) Frame;
    CurrAnim->NextFrame= (float) Frame;
    CurrAnim->Factor= 0;
    return 0;
}

LDEC(Instance_Animation_Set_Speed)
{
    struct LD_Instance_Struct * Instance =                           
        (struct LD_Instance_Struct*)            lua_topointer(L,1); 
    unsigned int BufferIndex =                  lua_tointeger(L,2); 
    float speed = lua_tonumber(L,3);   

    (Instance->Animation.Animations_Array+BufferIndex)->Speed=speed;
    return 0;
}

#define Instance_Animation_Set_Flag_Gen(x)                           \
   struct LD_Instance_Struct * Instance =                           \
        (struct LD_Instance_Struct*)            lua_topointer(L,1); \
    unsigned int BufferIndex =                  lua_tointeger(L,2); \
    char Bool = lua_toboolean(L,3);                                 \
    if(Bool) {*LD_3D_Instance_Animation_Flag(Instance,0) |= x;}     \
    else{*LD_3D_Instance_Animation_Flag(Instance,0) &= ~x;}    

LDEC(Instance_Animation_Set_Play)
{Instance_Animation_Set_Flag_Gen(1);return 0;}
LDEC(Instance_Animation_Set_Loop)
{Instance_Animation_Set_Flag_Gen(2);return 0;}
LDEC(Instance_Animation_Set_Interpolation)
{Instance_Animation_Set_Flag_Gen(8);return 0;}
LDEC(Instance_Animation_Set_Reverse)
{Instance_Animation_Set_Flag_Gen(4);return 0;}
LDEC(Instance_Animation_Set_Ignore)
{Instance_Animation_Set_Flag_Gen(0x80);return 0;}
#undef Instance_Animation_Set_Flag_Gen




/*********API STRUCT SETUP ***********/
typedef struct
{
    int (*GetKey) (int KeyCode);
} API_struct; 
 API_struct PhoAPI;
LDEC(Get_API)
{
    PhoAPI.GetKey = LD_GLFW_GetKey;
   
    lua_pushlightuserdata(L,&PhoAPI);
    return 1;
}
/********************************/

void Lua_add_registers(lua_State * L) //<- Definition
{
    LADD(Get_API)

    LADD(Instance_Animation_Set_Frame)
    LADD(Instance_Animation_Set_Speed)

    LADD(Instance_Animation_Set_Play);
    LADD(Instance_Animation_Set_Loop);
    LADD(Instance_Animation_Set_Interpolation);
    LADD_EXT(Instance_Animation_Set_Interpolation,"Instance_Animation_Set_Backwards");
    LADD(Instance_Animation_Set_Reverse);
    LADD(Instance_Animation_Set_Ignore);

    LADD(Entity_Get_Instance);

    LADD(Camera_add);
    LADD(Model_add);
    LADD(Animation_add);
    LADD(InstanceBuffer_Set_Capacity);
    LADD_EXT(InstanceBuffer_Set_Capacity,"VRAM_Instances");
    LADD(VRAMBuffers_allocate);
    LADD_EXT(VRAMBuffers_allocate,"VRAM_Buffers");
    LADD(VRAM_Instance_Create);
    LADD(Instance_Animations_Set_Capacity);
    LADD_EXT(Instance_Animations_Set_Capacity,"Instance_Animations");
    LADD(Instance_Set_Animation);
    LADD(Fill_VRAM_Buffer);
    LADD_EXT(Fill_VRAM_Buffer,"VRAM_Buffer_fill");
    LADD(Entity_Create);
    LADD(Entity_Set_VRAM_Instance);
    LADD(Entity_Set_VRAM_Scale);
    LADD(Scene_Set_Map);
    LADD(Script_Destroy);
    LADD(Script_Load);
    LADD(Script_This);
    LADD(Scene_Set_Script_File);
    LADD(Scene_Set_Script);
    LADD(Scene_Trigger_add);
    LADD(Scene_Find_Node_EXT);
    LADD(Scene_Find_Node);
    LADD(Entity_Teleport);
    LADD(Entity_Get_Position_EXT);
    LADD(Entity_Get_Position);
    LADD(Entity_Goto_EXT);
    LADD(Entity_Goto);
    LADD(Entity_Movement);
    LADD(Entity_Path_Set);
    LADD(Entity_Path_Clear);
    LADD(Entity_Get_Path);
    LADD(Entity_Path_Get_XY);
    LADD(Entity_Get_ID);
    LADD(Entity_Get_Data);
    LADD(Entity_Set_Data);
    LADD(Entity_Set_Sensor);
    LADD(Entity_Sensor_Check_Entity);
    LADD(Entity_Sensor_Update);
    LADD(Path_Find);
    LADD(Path_Destroy);
    LADD(VRAM_Buffer_Clear);
    LADD(Model_delete_all);
    LADD(Model_delete_Stack);


    Lua_LD_Math_add_registers(L);
	Lua_Utils_add_registers(L);
}





#undef LDEC
#undef LADD
#undef LADD_EXT

#endif

/*
int l_Pho_Camera_add(lua_State * L)
{
    Pho_Camera_add(lua_tonumber(L,1),lua_tonumber(L,2),lua_tonumber(L,3),
    lua_tonumber(L,4),lua_tonumber(L,5),lua_tonumber(L,6),lua_tonumber(L,7),
    lua_tonumber(L,8));
    return 1;
}

int l_Pho_Model_add(lua_State * L)
{
	IQM_RAW_Struct *  _Model_ = (IQM_RAW_Struct *) malloc(sizeof(IQM_RAW_Struct));
      	if(IQM_Load_File(_Model_,lua_tostring(L,1)) ==-1){printf("ERROR::MODEL\n");}
	LD_Model_Object_Struct * Model = LD_3D_ModelsLinkedList_Append((&_Model_->VertexCount));
	free(_Model_);
	lua_pushlightuserdata(L,&LD_3D.Models_LL->Last->Data);
	return 1;
}

int l_InstanceBuffer_Set_Capacity(lua_State *L)
{
	printf("VRAM Instances: %ld\n",lua_tointeger(L,1));
	LD_3D_InstanceBuffer_Set_capacity(lua_tointeger(L,1));
	return 1;
}

int l_VRAMBuffers_allocate(lua_State *L)
{
	printf("VRAM BUFFERS: %ld\n",lua_tointeger(L,1));
	LD_3D_VRAMBuffer_allocate(lua_tointeger(L,1));
	return 1;
}

int l_VRAM_Instance_Create(lua_State *L)
{
	//printf("Model: \n", (LD_Model_Object_Struct*)lua_topointer(L,2));
	struct LD_Model_Object_Struct*tmp = (struct LD_Model_Object_Struct*)lua_topointer(L,2);
	LD_3D_Fill_Instance(&LD_3D.InstancesBuffer[lua_tointeger(L,1)],tmp);

	lua_pushlightuserdata(L,&LD_3D.InstancesBuffer[lua_tointeger(L,1)]);
	return 1;
}

int l_Fill_VRAM_Buffer(lua_State *L)
{
	LD_3D_Fill_VRAMBuffer(&LD_3D.VRAMBuffer[lua_tointeger(L,1)],&LD_3D.InstancesBuffer[lua_tointeger(L,2)],
		&LD_3D.InstancesBuffer[lua_tointeger(L,3)],lua_tointeger(L,4));
	return 1;
}

int l_Pho_Entity_Create(lua_State * L)
{
    char * Function = NULL;
    if(lua_gettop(L)>8)
    {
	Function = (char*) lua_tostring(L,9);
    }

    _Entities_LL_ * RET =Entity_Create(
lua_tonumber(L,1),
lua_tonumber(L,2),
lua_tonumber(L,3),
lua_tonumber(L,4),
lua_tonumber(L,5),
lua_tonumber(L,6),
lua_tonumber(L,7),
(lua_State*)lua_topointer(L,8),
Function);

    if(lua_gettop(L)>9)
    {
	Entity_set_Model_Instance(RET,
	(struct LD_Instance_Struct *)lua_topointer(L,10));
    }
    
    if(lua_gettop(L)>10)
    {
	Entity_set_Model_Instance_Scale(RET,lua_tonumber(L,11));
    }

    lua_pushlightuserdata(L,RET);
   return 1; 
}

int l_Pho_Entity_Set_VRAM_Instance(lua_State * L)
{
    Entity_set_Model_Instance(
(struct _Entities_LL_ *)lua_topointer(L,1),
(struct LD_Instance_Struct *)lua_topointer(L,2));
    return 1;
}

int l_Pho_Entity_Set_VRAM_Scale(lua_State *L)
{
    Entity_set_Model_Instance_Scale((_Entities_LL_*)lua_topointer(L,1),lua_tonumber(L,2));
    return 1;
}

int l_Scene_Set_Map(lua_State * L)
{
    int RET = Scene_Set_Map_STLbin((char*)lua_tostring(L,1));
    
    lua_pushinteger(L,RET);
    return 1;
}

void Lua_add_registers(lua_State *L); //<- Only declaration

int l_Script_Destroy(lua_State *L)
{
    lua_State * SCRIPT = (lua_State *) lua_topointer(L,1);
    lua_close(SCRIPT);
    return 0;
}

int l_Script_Load(lua_State *L)
{
    lua_State * RET=luaL_newstate();
    luaL_openlibs(RET);
    int err =luaL_loadfile(RET,(char*)lua_tostring(L,1));
    
    if(err!=0)
    {
	lua_close(RET);
	RET=NULL;
    }

    Lua_add_registers(RET);
    lua_pcall(RET,0,0,0);
    lua_pushlightuserdata(L,RET);
    return 1;
}

int l_Script_This(lua_State *L)
{
    lua_pushlightuserdata(L,L);
    return 1;
}

int l_Scene_Set_Script_File(lua_State *L)
{
    Scene_Set_Lua_Script((char*)lua_tostring(L,1));
    Lua_add_registers(Pho_Scene.Lua_Script);
    lua_pcall(Pho_Scene.Lua_Script,0,0,0);
    return 0;
}

int l_Scene_Set_Script(lua_State *L)
{
    Pho_Scene.Lua_Script = (lua_State*) lua_topointer(L,1);
    return 0;
}

int l_Scene_Trigger_add(lua_State*L)
{
	float Segment[] = 
	{lua_tonumber(L,1),lua_tonumber(L,2),
	lua_tonumber(L,3),lua_tonumber(L,4)};
	
	void * OverPTR = (void *) lua_topointer(L,6);
	
	lua_pushlightuserdata(L,
	Scene_Trigger_Add(Segment,Segment+2,
	(char*)lua_tostring(L,5),OverPTR));
	return 1;
}

int l_Scene_Find_Node_EXT(lua_State *L)
{
    float Point_2D[]={lua_tonumber(L,1),lua_tonumber(L,2)};
    lua_pushlightuserdata(L,Scene_Get_Node(Point_2D));
    return 1;
}

int l_Scene_Find_Node(lua_State *L)
{
    lua_pushlightuserdata(L,Scene_Get_Node((float*)lua_topointer(L,1)));
    return 1;
}

int l_Entity_Teleport(lua_State * L)
{
	struct __Movement_ * Mov = (struct __Movement_*)
	&(((struct _Entities_LL_*) lua_topointer(L,1))->Entity.Movement);
	Mov->HitBox->AABB.Center_Position[0] = lua_tonumber(L,2);
	Mov->HitBox->AABB.Center_Position[1] = lua_tonumber(L,3);
	Mov->Direction_Degree = lua_tonumber(L,4);
	return 1;
}

int l_Entity_Get_Position(lua_State *L)
{
    float * Position =
    ((_Entities_LL_*)lua_topointer(L,1))->Entity.Movement.HitBox->AABB.Center_Position;
    lua_pushlightuserdata(L,Position);
    return 1;
}

int l_Entity_Get_Position_EXT(lua_State *L)
{
    float * Position =
    ((_Entities_LL_*)lua_topointer(L,1))->Entity.Movement.HitBox->AABB.Center_Position;
    lua_pushnumber(L,Position[0]);
    lua_pushnumber(L,Position[1]);
    return 2;
}

int Angles_Check_Side(float Angle, float Test) //Needs ASM implementation
{
    float Diff =  Test - Angle;

    if(Diff < -180.0f){Diff += 360.f;}
    if(Diff > 180.0f){Diff -= 360.f;}

    if(Diff > 0.0f){return 1;}
    if(Diff < 0.0f){return -1;}

    return 0;
}

float Angle_Diff(float Angle, float Test) //Needs ASM implementation
{
    return 180.f - fabsf(fabsf(Angle - Test)-180.f);
}

float Angle_0_360(float Angle) //Needs ASM implementation
{
    return Angle-((floor(Angle/360.f))*360.f);
}

char __ENTITY_GOTO_(struct _Entities_LL_ * Entity,float * Point2D, float Forward,float Turn)
{
    float * Position =Entity->Entity.Movement.HitBox->AABB.Center_Position;
    float Delta = CHRONO_STRUCT.DELTA_TIME.Delta_Time;
    float DistanceToMove = Entity->Entity.Movement.Mov_Speed*Forward*Delta;
    float DistanceToPoint =V2Distance(Position,Point2D);
    if(DistanceToPoint<=DistanceToMove)
    {memcpy(Position,Point2D,8);Forward=0;return 1;}
    
	float AngleToPoint;V2DegreesV2_FPU(Position,Point2D,&AngleToPoint);
	float Angle = Entity->Entity.Movement.Direction_Degree;
	Angle = Angle_0_360(Angle);
	AngleToPoint = Angle_0_360(AngleToPoint);
	
	float AngleToTurn = Entity->Entity.Movement.Turn_Speed*Turn*Delta; 
	float AngleDiff = Angle_Diff(Angle,AngleToPoint);
	
    if( Turn>= 0)
    {

	if((AngleDiff) <= AngleToTurn)
	{
	    if(AngleToTurn != 0)
	    {Turn = AngleDiff / AngleToTurn;}
	    else{Turn=0;}
	}   
	if(Angles_Check_Side(Angle,AngleToPoint)==-1){Turn*=-1;}
//	printf("Ang: %f | Diff: %f | Turn: %f | %f\n",Angle,AngleDiff,AngleToTurn,Turn);
    }
    else
    {
	Turn=0;
	Entity->Entity.Movement.Direction_Degree=AngleToPoint;
    }
	//printf("%f | %f \n",Angle,AngleToPoint);
       Entity_Movement(Entity,Forward,Turn);
    return 0;
}

int l_Entity_Goto(lua_State *L)
{
    struct _Entities_LL_ * Entity =(_Entities_LL_*)lua_topointer(L,1);
    float * Point2D=(float *)lua_topointer(L,2);
    float Forward = 1.f, Turn = -1;
    if(lua_gettop(L) > 3)
    {Forward = lua_tonumber(L,4);}
    if(lua_gettop(L) > 4)
    {Turn = lua_tonumber(L,5);}
    char RET = __ENTITY_GOTO_(Entity,Point2D,Forward,Turn); 
    lua_pushboolean(L,RET);
    return 1;
}

int l_Entity_Goto_EXT(lua_State *L)
{
    struct _Entities_LL_ * Entity =(_Entities_LL_*)lua_topointer(L,1);
    float Point2D[]={lua_tonumber(L,2),lua_tonumber(L,3)};
    float Forward = 1.f, Turn = -1;
    if(lua_gettop(L) > 3)
    {Forward = lua_tonumber(L,4);}
    if(lua_gettop(L) > 4)
    {Turn = lua_tonumber(L,5);}
    char RET = __ENTITY_GOTO_(Entity,Point2D,Forward,Turn); 
    lua_pushboolean(L,RET);
    return 1;
}

int l_Pho_Entity_Movement(lua_State * L)
{
    Entity_Movement((struct _Entities_LL_*)lua_topointer(L,1),
    lua_tonumber(L,2),lua_tonumber(L,3));
    return 1;
}

int l_Entity_Path_Set(lua_State *L)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    struct Path_2D_struct * Path = (struct Path_2D_struct*)lua_topointer(L,2);
    Entity->Entity.AI.PathFinder.Path = Path;
    return 1;
}

int l_Entity_Path_Clear(lua_State *L)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    char Erase = 1;
    if(lua_gettop(L)>1){Erase = lua_tointeger(L,2);}
    if(Erase == 1){Path_2D_Destroy(Entity->Entity.AI.PathFinder.Path);}
    Entity->Entity.AI.PathFinder.Path=NULL;
    return 1;
}

int l_Entity_Get_Path(lua_State *L)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    Path_2D_struct* PATH =Entity->Entity.AI.PathFinder.Path;
    //if(PATH == NULL){printf("NULL\n");}
    if(PATH == NULL){return 0;}
    lua_pushlightuserdata(L,PATH);
    return 1;
}

int l_Entity_Path_Get_XY(lua_State *L)
{
    struct _Entities_LL_ * Entity = (struct _Entities_LL_*)lua_topointer(L,1);
    float Point2D[2];
    Entity_AI_Evaluate_Path(Entity,Point2D);
    //printf("%f,%f\n",Point2D[0],Point2D[1]);
    lua_pushnumber(L,(float) Point2D[0]);
    lua_pushnumber(L,(float) Point2D[1]);
    return 2;
}

int l_Entity_Get_ID(lua_State *L)
{
    lua_pushinteger(L,((struct _Entities_LL_*)lua_topointer(L,1))->Entity.ID );
    return 1;
}

int l_Entity_Get_Data(lua_State *L)
{
    lua_pushlightuserdata(L,((struct _Entities_LL_*)lua_topointer(L,1))->Entity.Data);
    return 1;
}

int l_Entity_Set_Data(lua_State *L)
{
    ((struct _Entities_LL_*)lua_topointer(L,1))->Entity.Data   ;
    return 1;
}

int l_Entity_Set_Sensor(lua_State *L)
{
    struct _Entities_LL_*Entity = ((struct _Entities_LL_*)lua_topointer(L,1));
    Entity_set_Sensor_Parameters(Entity,lua_tonumber(L,2),lua_tonumber(L,3));
    return 0;
}

int l_Entity_Sensor_Check_Entity(lua_State *L)
{
    char Bool = 0;
    struct _Entities_LL_* Caller = (struct _Entities_LL_*) lua_topointer(L,1);
    struct _Entities_LL_* Test = (struct _Entities_LL_*) lua_topointer(L,2);
    Bool=Entity_AI_Sensor_Check_Entity(Caller,Test);
    lua_pushboolean(L,Bool);
    return 1;
}

int l_Entity_Sensor_Update(lua_State*L)
{
    char Boolean =Entity_AI_Sensor_Update( (struct _Entities_LL_*) lua_topointer(L,1));
    lua_pushboolean(L,Boolean);
    return 1;
}

int l_Path_Find(lua_State * L)
{
    Path_2D_struct *RET=NULL;
    RET=Navi_Map_2D_FindPath((_NavNode_2D_*)lua_topointer(L,1),(_NavNode_2D_*)lua_topointer(L,2),lua_tonumber(L,3));

    int varc = lua_gettop(L);

    if(varc > 3) //Destiny has been especified//
    {
	if(varc>4)
	{
	    RET->Destiny[0] = lua_tonumber(L,4);
	    RET->Destiny[1] = lua_tonumber(L,5);
	}
	else{memcpy(RET->Destiny,(float*) lua_topointer(L,4),8);}
    }

    lua_pushlightuserdata(L,RET);
    return 1;
}

int l_Path_Destroy(lua_State *L)
{
    Path_2D_Destroy((Path_2D_struct*)lua_topointer(L,1));
    return 1;
}

int l_VRAM_Buffer_Clear(lua_State * L)
{
	LD_3D_VRAMBuffer_Clear(&LD_3D.VRAMBuffer[lua_tointeger(L,1)],1);
	return 1;
}

int l_Model_delete_all(lua_State * L)
{
	LD_3D_ModelsLinkedList_System_Delete(NULL);
	return 1;
}

int l_Model_delete_Stack(lua_State * L)
{
	LD_3D_ModelsLinkedList_System_Delete((struct LD_Model_Object_Struct *)lua_tointeger(L,1));
	return 1;
}

void Lua_add_registers(lua_State * L) //<- Definition
{

	lua_register(L,"Script_Load",l_Script_Load);
	lua_register(L,"Script_Destroy",l_Script_Destroy);
	lua_register(L,"Script_This",l_Script_This);
	

	//VRAM_Buffer_fill(int Buffer_ID_number, int First_Instance_ID, int Last_Instance_ID, char Static_Flag);
	lua_register(L,"VRAM_Buffer_fill",l_Fill_VRAM_Buffer);
	//VRAM_Instance_Create(int Instance_ID_Number, void * Model)
	lua_register(L,"VRAM_Instance_Create",l_VRAM_Instance_Create);
	lua_register(L,"VRAM_Instances",l_InstanceBuffer_Set_Capacity);
	lua_register(L,"VRAM_Buffers",l_VRAMBuffers_allocate);
	//VRAM_Buffer_Clear(int VRAM_ID)
	lua_register(L,"VRAM_Buffer_Clear",l_VRAM_Buffer_Clear);
	lua_register(L,"Camera_add",l_Pho_Camera_add);
	lua_register(L,"Model_add",l_Pho_Model_add);
	//Model_delete_Stack(Model_to_start);
	lua_register(L,"Model_delete_Stack",l_Model_delete_Stack);
	lua_register(L,"Model_delete_all",l_Model_delete_all);

	//Scene_Set_Map(STL_File_Path);
	lua_register(L,"Scene_Set_Map",l_Scene_Set_Map);
	//Scene_Set_Script_File(File_Path);	
	lua_register(L,"Scene_Set_Script_File",l_Scene_Set_Script_File);
	//Scene_Set_Script(lua_State);	<- It uses a preloaded Lua State
	lua_register(L,"Scene_Set_Script",l_Scene_Set_Script);
	//Scene_Trigger_add(A_x,A_y,B_x,B_y,FunctionName_String,Parameters_Pointer);
	lua_register(L,"Scene_Trigger_add",l_Scene_Trigger_add);
	//Scene_Get_Node_EXT(X,Y);
	lua_register(L,"Scene_Find_Node_EXT",l_Scene_Find_Node_EXT);
	//Scene_Get_Node(Point2D);
	lua_register(L,"Scene_Find_Node",l_Scene_Find_Node);

	//Path_Find(Node_A, Node_B, Radius) <- It returns a Path
	//Path_Find(Node_A, Node_B, Radious, Destiny_X, Destiny_Y);
	lua_register(L,"Path_Find",l_Path_Find);
	//Path_Destroy(Path)
	lua_register(L,"Path_Destroy",l_Path_Destroy);
	

	//Entity_Create(int ID,float X, float Y, float Direction, float Speed, float Turn_Speed, float Hitbox_half_size,script,function);
	//Entity_Create(int ID,float X, float Y, float Direction, float Speed, float Turn_Speed, float Hitbox_half_size,script,function,VRAM_Instance);
	//Entity_Create(int ID,float X, float Y, float Direction, float Speed, float Turn_Speed, float Hitbox_half_size,script,function,VRAM_Instance,VRAM_Instance_Scale);
	lua_register(L,"Entity_Create",l_Pho_Entity_Create);
	//Entity_Set_VRAM_Instance(Entity, VRAM_Instance);
	lua_register(L,"Entity_Set_VRAM_Instance",l_Pho_Entity_Set_VRAM_Instance);
	lua_register(L,"Entity_Set_VRAM_Instance_Scale",l_Pho_Entity_Set_VRAM_Scale);
	//Entity_Movement(Entity,Forward,turn);
	lua_register(L,"Entity_Movement",l_Pho_Entity_Movement);
	//Entity_Teleport(Entity,new_X,new_Y,new_Direction)
	lua_register(L,"Entity_Teleport",l_Entity_Teleport);
	//float * Entity_Get_Position(Entity);  <- It returns a 2D vector 
	lua_register(L,"Entity_Get_Position",l_Entity_Get_Position);
	//X,Y Entity_Get_Position_EXT(Entity);  <- It returns two floats, they get pushed into lua 
	lua_register(L,"Entity_Get_Position_EXT",l_Entity_Get_Position_EXT);
	//int Entity_Get_ID(Entity); <- It returns the ID of the entity;
	lua_register(L,"Entity_Get_ID",l_Entity_Get_ID);	
	//Entity_Set_Sensor(Entity,Parameter_1,Parameter_2)
	lua_register(L,"Entity_Set_Sensor",l_Entity_Set_Sensor);
	//Entity_Sensor_Check_Entity(Entity, Target) <-Check if the target is visible.
	lua_register(L,"Entity_Sensor_Check_Entity",l_Entity_Sensor_Check_Entity);
	//Entity_Sensor_Update(Entity) <- It updates the Entity's sensor
	lua_register(L,"Entity_Sensor_Update",l_Entity_Sensor_Update);
	//Entity_Set_Data//
	lua_register(L,"Entity_Set_Data",l_Entity_Set_Data);
	//Entity_Get_Data//
	lua_register(L,"Entity_Get_Data",l_Entity_Get_Data);
	//Entity_Goto_EXT(Entity,X,Y);
	//Entity_Goto_EXT(Entity,X,Y,Speed_Multiplier);
	lua_register(L,"Entity_Goto_EXT",l_Entity_Goto_EXT);
	//Entity_Goto(Entity,Point2D);
	//Entity_Goto(Entity,Point2D,Speed_Multiplier);
	lua_register(L,"Entity_Goto",l_Entity_Goto);
	//Entity_Path_Set(Entity,Path, Destiny_Point2D)
	//Entity_Path_Set(Entity,Path,Destiny_X,Destiny_Y)
	lua_register(L,"Entity_Path_Set",l_Entity_Path_Set);
	//Entity_Path_Clear(Entity) <- It destroys the Entity's AI Path
	lua_register(L,"Entity_Path_Clear",l_Entity_Path_Clear);
	//Path = Entity_Get_Path(Entity) <- It returns the Path, null is not
	lua_register(L,"Entity_Get_Path",l_Entity_Get_Path);
	//X,Y = Entity_Path_Get_XY(Entity) <- It returns coordinates to follow the path
	lua_register(L,"Entity_Path_Get_XY",l_Entity_Path_Get_XY);

	Lua_LD_Math_add_registers(L);
	Lua_Utils_add_registers(L);
}

#endif
*/