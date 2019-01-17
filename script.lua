--[[Testing]]--
--[[-Luis Delgado. 2018]]--
print("Starting LUA")

MaxInstances=2

VRAM_Instances(MaxInstances+2); -- <-- The floor is an instance too.
VRAM_Buffers(2);

print("LOADED 0");

Character_model =Model_add("body.iqm");
Character_anim = Animation_add("body_Walk.iqm");

print("LOADED");

Model = VRAM_Instance_Create(0,Character_model);
Instance_Animations_Set_Capacity(Model,1);
Instance_Set_Animation(Model,0,Character_anim,0);
Instance_Animation_Set_Loop(Model,0,true);
Instance_Animation_Set_Interpolation(Model,0,true);
Jugador = Entity_Create(0,0,0,0,0.1,4,0.25,Script_This(),"PC",Model);

print("---");

Camera_add(10,-10,10,0,0,5,60,1400);

print("---");

VRAM_Buffer_fill(0,0,0,0);

print("---");





local ffi = require("ffi");
ffi.cdef[[
typedef struct
{
    int (*GetKey) (int KeyCode);
} API_struct;
]]

PhoAPI = ffi.cast("API_struct *",Get_API());

--AI--

    --Player--
    function PC_GetMov()
        --Keyboard Movement--
        local izq = PhoAPI.GetKey(263);
        local der = PhoAPI.GetKey(262);
        local forw = PhoAPI.GetKey(265);
        local backw = PhoAPI.GetKey(264);
        return forw-backw, izq-der ;
        
    end
    function PC (Caller)
        Instance = Entity_Get_Instance(Caller);
        --Instance_Animation_Set_Ignore(Instance,0,true)
        Instance_Animation_Set_Play(Instance,0,false);

        local M,T = PC_GetMov()
        if(M ~= 0 or T ~= 0) then
            --Instance_Animation_Set_Ignore(Instance,0,false)
            Instance_Animation_Set_Play(Instance,0,true);
            Instance_Animation_Set_Reverse(Instance,0,(M<0));
        else 
            Instance_Animation_Set_Frame(Instance,0,0)
        end

        Entity_Movement(Caller,M,T);

    end
