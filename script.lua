print("Lua Test");
VRAM_Instances(3);
VRAM_Buffers(2);


Prota_model =Model_add("mono.iqm");
Piso_model = Model_add("test1.iqm");
Scene_Set_Map("test1.stl");
Scene_Set_Script("Triggers.lua");


Prota_VRAM = VRAM_Instance_Create(0,Prota_model);
VRAM_Instance_Create(1,Piso_model);
VRAM_Buffer_fill(0,0,1,0);

VRAM_Buffer_Clear(0)

VRAM_Instance_Create(1,Piso_model);
VRAM_Buffer_fill(0,0,1,0);

--VRAM_Buffer_fill(1,1,1,1);

--Camera_add(-20,-20,20,0,0,0,50,500);
Camera_add(-2,-9,3,0,0,0,60,20);
Camera_add(8,-5,3,4,0,0,60,10);
Camera_add(8,-5,3,12,0,0,60,10);





Jugador = Entity_Create(0,-3,0,0.30,10,0.25,Prota_VRAM);


--otro = Entity_Create(0,0,0,0.30,10,0.25);

--Scene_Trigger_add(-1000,0,1000,0,"TP",nil)