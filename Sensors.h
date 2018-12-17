#ifndef _PHO_SENSORS_H_
#define _PHO_SENSORS_H_

//- Luis Delgado. 2018

struct Sensor_struct
{
    float Parameter_1; //This parameter describes the capability of the sensor
    float Parameter_2; //This parameter too 
    int Buffers_Count;

    struct _Sensor_Buffer_
    {
	int Data_Counter;
	struct _Sensor_Data_LL_
	{
	    void * Pointer;
	    struct _Sensor_Data_LL_ * Next;
	} * First;
	struct _Sensor_Data_LL_ * Last;
    }*Buffer_Array;
};


char Sensor_allocate(struct Sensor_struct * Sensor,int Buffer_Count)
{
    struct _Sensor_Buffer_ * tmp = (struct _Sensor_Buffer_*)
    malloc(sizeof(struct _Sensor_Buffer_)*Buffer_Count);
    if(tmp==NULL){return 0;}
    Sensor->Buffers_Count = Buffer_Count;
    Sensor->Buffer_Array=tmp;
    struct _Sensor_Buffer_ * END = tmp+Buffer_Count;
    for(;tmp<END;tmp++)
    {tmp->Data_Counter=0;}
    
    return 1;
}

void Sensor_Set_Parameters(struct Sensor_struct*Sensor,float Parameter_1,float Parameter_2)
{
    Sensor->Parameter_1 = Parameter_1;Sensor->Parameter_2=Parameter_2;
}

void Sensor_Clear(struct Sensor_struct * Sensor)
{
    int *BC = &Sensor->Buffers_Count;
    if(*BC == 0){return;}

    for(int i = 0;i<*BC;i++)
    {
	struct _Sensor_Data_LL_ * Ex = (Sensor->Buffer_Array+i)->First;
	struct _Sensor_Data_LL_ * tmp;
	while(Ex!=NULL)
	{
	    tmp = Ex;
	    free(Ex);
	    Ex=tmp->Next;
	}
    }
    *BC = 0;
}

char Sensor_reallocate(struct Sensor_struct * Sensor, int Buffer_Count)
{
    if(Buffer_Count <= 0){Sensor_Clear(Sensor);return 1;}
       
    struct _Sensor_Buffer_ * tmp = (struct _Sensor_Buffer_*)
    realloc(Sensor->Buffer_Array,sizeof(struct _Sensor_Buffer_)*Buffer_Count);
    if(tmp == NULL){return 0;} 

    Sensor->Buffer_Array = tmp;    
    
    if(Buffer_Count > Sensor->Buffers_Count)
    {
	struct _Sensor_Buffer_ * END = tmp+Buffer_Count;
	for(;tmp<END;tmp++)
	{tmp->Data_Counter=0;}
    }

    Sensor->Buffers_Count = Buffer_Count;
    return 1;
}

struct _Sensor_Data_LL_ * Sensor_add(struct Sensor_struct * Sensor,int Buffer_ID, void * Pointer)
{
    if(Buffer_ID<0 || Buffer_ID>Sensor->Buffers_Count-1){return NULL;}

    struct _Sensor_Buffer_ * ptr = Sensor->Buffer_Array+Buffer_ID;
    int * DC = &ptr->Data_Counter;

    if(*DC==0)
    {
	ptr->First= (struct _Sensor_Data_LL_*)
	malloc(sizeof(struct _Sensor_Data_LL_));
	ptr->Last = ptr->First;
    }
    else
    {
	ptr->Last->Next= (struct _Sensor_Data_LL_*)
	malloc(sizeof(struct _Sensor_Data_LL_));
	ptr->Last =ptr->Last->Next;
    }
    
    ptr->Last->Next=NULL;
    ptr->Last->Pointer = Pointer;
    *DC+=1;
    return ptr->Last;
}

//Check a point in the sensor as if the last had a Field of View
char Sensor_Check_as_FOV_2D(struct Sensor_struct * Sensor,
float * Sensor_Position,float Sensor_Degrees,float * Point_2D)
{
    float FOV_Degrees  = Sensor->Parameter_1;
    float Max_Distance = Sensor->Parameter_2;

    char In_Distance =(V2Distance(Sensor_Position,Point_2D) <= Max_Distance);

    float Ang; V2DegreesV2_FPU(Sensor_Position,Point_2D,&Ang);
    char In_FOV =(180.f - abs(abs(Sensor_Degrees - Ang) - 180.f) <= (FOV_Degrees*0.5));

    if(In_Distance && In_FOV)
    {
	return 1;
    }
    return 0;
}

#endif
