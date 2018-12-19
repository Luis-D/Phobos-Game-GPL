--[[Testing]]--
--[[-Luis Delgado. 2018]]--

MaxInstances=10

VRAM_Instances(MaxInstances+2); -- <-- The floor is an instance too.
VRAM_Buffers(2);

Character_model =Model_add("Human_Placeholder.iqm");
Floor_model = Model_add("Lab1.iqm");

CharaSize = 0.5

--[[Load Player]]--
Model = VRAM_Instance_Create(1,Character_model);
Jugador = Entity_Create(0,4,0,0,0.1,10,0.25,nil,nil,Model,CharaSize);

--[[Load NPCs]]--
for i=2,MaxInstances do
    Model = VRAM_Instance_Create(i,Character_model);
    NPC = Entity_Create(i-1,-2.3+(i*0.05),0,-90+i,0.04,5,0.15,Script_This(),"stalk",Model,CharaSize);
    Entity_Set_Sensor(NPC,80,7)
end

--[[Load World]]--
Scene_Set_Map("Lab1.stl");
Scene_Set_Script(Script_This());
Camera_add(-4,14,3,-4,0,0,60,14);
Camera_add(9,9,3,6,6,0,60,10);
--Camera_add(-2,-12,3, 0,0,0,60,14);
Camera_add(1, -12,3,3,0,0,45,16);
Camera_add(-4, -13,3,-3,0,0,45,16);

Scene_Trigger_add(2,-3,5,-3,"TP",nil)
Scene_Trigger_add(-2,0.5,-2,-0.5,"TP2",nil)

VRAM_Instance_Create(0,Floor_model);




--[[Graphic stuff]]--
VRAM_Buffer_fill(0,0,0,1);
VRAM_Buffer_fill(1,1,MaxInstances,0);


--[[Functions]]--

function stalk (caller)
    Entity_Path_Clear(caller);
    v=Entity_Sensor_Check_Entity(caller,Jugador)
    if(1==0) then
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
        Entity_Teleport(Caller,3.5,2,250)
    end 
	
end

function TP2(Caller,Parameter)
    
    Entity_Teleport(Caller,-5,-3,0)
    
	
end

