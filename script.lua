print("Lua Test");
VRAM_Instances(100);
VRAM_Buffers(2);

Prota_model =Model_add("mono.iqm");
Piso_model = Model_add("test1.iqm");
Scene_Set_Map("test1.stl");
Scene_Set_Script("Triggers.lua");

Camera_add(-2,-9,3,0,0,0,60,20);
Camera_add(8,-5,3,4,0,0,60,10);
Camera_add(8,-5,3,12,0,0,60,10);

--[[Load World]]--
VRAM_Instance_Create(0,Piso_model);
VRAM_Buffer_fill(0,0,0,1);


--[[Load Player]]--
Model = VRAM_Instance_Create(1,Prota_model);
Jugador = Entity_Create(0,0,0,0,0.2,10,0.25,nil,nil,Model,0.2);


--[[Load NPCs]]--
o=3
for i=2,o do
    Model = VRAM_Instance_Create(i,Prota_model);
    Entity_Create(2,-2+(i*0.05),-2,0,0.02,10,0.25,Script_This(),"stalk",Model,0.2);
end
VRAM_Buffer_fill(1,1,o,0);


function stalk (caller)
--    print("Start loop");
           Entity_Path_Clear(caller);
    PP = Entity_Get_Position(Jugador);
    CalP= Entity_Get_Position(caller);
    DD = V2Distance(PP,CalP);
    if(DD<1) then
	Entity_Movement(caller,0,0);
	return 
    end
	    Node=Scene_Find_Node(CalP)   
	    Node2 = Scene_Find_Node(PP);
--	print(" -> ("..Px..","..Py..")");
--	print(Node);print("->");print(Node2);
            PATH = Path_Find(Node,Node2,0,PP);
--	print(PATH)    
	Entity_Path_Set(caller,PATH);
            X,Y = Entity_Path_Get_XY(caller);
            if Entity_Goto_EXT(caller,X,Y,1) then
                Entity_Path_Clear(caller);
                Entity_Movement(caller,0,0);
            end 
--	print("End Loop");
end
