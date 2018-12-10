#include "libLDCC/LD_Geometry.h"

#define FunctionNameSize 8

struct Pho_Triggers_LL_
{
    struct _Trigger_Node_
    {
        float Point_A[2];
        float Point_B[2];
        
        struct _Trigger_Node_ * Next;
        void * Parameter_Pointer;
        
        char Function[FunctionNameSize]; //Function names can only have 7 ASCII symbols plus a \0;
    } * First;
    struct _Trigger_Node_ * Last;
};

struct _Trigger_Node_ * Pho_Trigger_Add
(struct Pho_Triggers_LL_ * LinkedList, 
float * Point_A,float * Point_B,
char * Function_Name,void * Parameters_pointer)
{
    if(LinkedList->Last==NULL)
    {
	LinkedList->First = (struct _Trigger_Node_ *) malloc(sizeof(struct _Trigger_Node_));
	LinkedList->Last = LinkedList->First;
    } 
    else
    {
	LinkedList->Last->Next=(struct _Trigger_Node_*)malloc(sizeof(struct _Trigger_Node_));
	LinkedList->Last = LinkedList->Last->Next;
    }
    LinkedList->Last->Next=NULL;

    memcpy(LinkedList->Last->Function,Function_Name,FunctionNameSize);
    LinkedList->Last->Function[FunctionNameSize-1]=0; //<- Set \0 as the last character
    if(Point_A!=NULL){memcpy(LinkedList->Last->Point_A,Point_A,sizeof(float)*2);}
    if(Point_B!=NULL){memcpy(LinkedList->Last->Point_B,Point_B,sizeof(float)*2);}

    return LinkedList->Last;
}

void Pho_Trigger_Clear_All(struct Pho_Triggers_LL_ * LinkedList)
{
    struct _Trigger_Node_* Exp = LinkedList->First;
    struct _Trigger_Node_* tmp;    

    LinkedList->First=NULL;
    LinkedList->Last=NULL;
    
    while(Exp!=NULL)
    {
        tmp = Exp->Next;
        free(Exp);
        Exp = tmp;	
    }
}

void Pho_Trigger_Execute(struct _Trigger_Node_ * Trigger,lua_State * Lua_Script,void * Caller_Pointer,void * Overrider_Pointer)
{
    if(Overrider_Pointer==NULL)
    {Overrider_Pointer = Trigger->Parameter_Pointer;}
    lua_getglobal(Lua_Script,Trigger->Function);
    lua_pushlightuserdata(Lua_Script,Caller_Pointer);
    lua_pushlightuserdata(Lua_Script,Overrider_Pointer);
    lua_pcall(Lua_Script,2,0,0);
}   

#define FunctionPrototypes char (*Checker_Func) (struct _Trigger_Node_ *,float*,lua_State *,void*,void*)

char Pho_Trigger_Check_as_Segment_vs_Segment(struct _Trigger_Node_ * Trigger,
float * Segment,lua_State * Lua_Script,void * Caller_Pointer, void * Overrider_Pointer)
{
    float T = 0;
   // printf("%f,%f,%f,%f | (%f,%f)\n",Segment[0],Segment[1],Segment[2],Segment[3],Segment[2]-Segment[0],Segment[3]-Segment[1]);
    char Col = Check_Segment_vs_Segment_2D(Segment,Trigger->Point_A,(float*)&T);
 //printf("%f\n",T);
    if(Col)
    {
       
        //printf("Col: %f,%f,%f,%f (%f)\n",Trigger->Point_A[0],Trigger->Point_A[1],Trigger->Point_A[2],Trigger->Point_A[3],T);
        Pho_Trigger_Execute(Trigger,Lua_Script,Caller_Pointer,Overrider_Pointer);return 1;
    }
    return 0;
}



void Pho_Triggers_Test_LinkedList(struct Pho_Triggers_LL_ * LinkedList,FunctionPrototypes,lua_State * Lua_Script,
float * Test_Against,void * Caller_Pointer)
{
    struct _Trigger_Node_ * Ex= LinkedList->First;

    while(Ex!=NULL)
    {
        Checker_Func(Ex,Test_Against,Lua_Script,Caller_Pointer,NULL);
        Ex=Ex->Next;
    }
}