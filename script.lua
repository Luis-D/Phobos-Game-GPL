--[[Testing]]--
--[[-Luis Delgado. 2018]]--

MaxInstances=2

VRAM_Instances(MaxInstances+2); -- <-- The floor is an instance too.
VRAM_Buffers(2);

Character_model =Model_add("body.iqm");

print("LOADED");

Model = VRAM_Instance_Create(0,Character_model);
print(Model)
Jugador = Entity_Create(0,0,0,0,0.1,4,0.25,nil,nil,Model);

print("---");

Camera_add(10,10,10,0,0,5,60,1400);

print("---");

VRAM_Buffer_fill(0,0,0,0);

print("---");
