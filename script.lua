--[[Testing]]--
--[[-Luis Delgado. 2018]]--

MaxInstances=100

VRAM_Instances(MaxInstances+2); -- <-- The floor is an instance too.
VRAM_Buffers(2);

Character_model =Model_add("Human_Placeholder.iqm");
Floor_model = Model_add("test1.iqm");


--[[Load World]]--
Scene_Set_Map("test1.stl");
Scene_Set_Script(Script_This());
Camera_add(-2,-9,3,0,0,0,60,20);
Camera_add(8,-5,3,4,0,0,60,10);
Camera_add(8,-5,3,12,0,0,60,10);
Scene_Trigger_add(7,0,12,0,"TP",nil)
VRAM_Instance_Create(0,Floor_model);


CharaSize = 0.5

--[[Load Player]]--
Model = VRAM_Instance_Create(1,Character_model);
Jugador = Entity_Create(0,1,2,0,0.1,10,0.25,nil,nil,Model,CharaSize);

--[[Load NPCs]]--
for i=2,MaxInstances do
    Model = VRAM_Instance_Create(i,Character_model);
    NPC = Entity_Create(i-1,-1,-0-(i*0.02),-90+i,0.02,5,0.25,Script_This(),"stalk",Model,CharaSize);
    Entity_Set_Sensor(NPC,80,7)
end

--[[Graphic stuff]]--
VRAM_Buffer_fill(0,0,0,1);
VRAM_Buffer_fill(1,1,MaxInstances,0);


--[[Functions]]--

function stalk (caller)
    Entity_Path_Clear(caller);
    v=Entity_Sensor_Check_Entity(caller,Jugador)
    if(not v) then
	Entity_Movement(caller,0,0);
    else
	PP = Entity_Get_Position(Jugador);
	CalP= Entity_Get_Position(caller);
	DD = V2Distance(PP,CalP);
	Node=Scene_Find_Node(CalP)   
	Node2 = Scene_Find_Node(PP);
	PATH = Path_Find(Node,Node2,0,PP);
	Entity_Path_Set(caller,PATH);
	X,Y = Entity_Path_Get_XY(caller);
	if Entity_Goto_EXT(caller,X,Y,1,1) then
		Entity_Movement(caller,0,0);
    	end
    end
end

function TP(Caller,Parameter)
    ID = Entity_Get_ID(Caller)
    if ID == 0 then -- <-Check if the caller is the Player
        Entity_Teleport(Caller,0,-2,40)
    end 
end

