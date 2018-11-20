print("Lua Test");
VRAM_Instances(3);
VRAM_Buffers(2);


Prota_model =Model_add("mono.iqm");
Piso_model = Model_add("test1.iqm");

VRAM_Instance_Create(0,Prota_model);
VRAM_Buffer_fill(0,0,0,0);
VRAM_Instance_Create(1,Piso_model);
VRAM_Buffer_fill(1,1,1,1);

Camera_add(0,-2,10,0,0,0,50,5);
Camera_add(8,-5,3,4,0,0,90,4);
Camera_add(8,-5,3,12,0,0,90,4);
