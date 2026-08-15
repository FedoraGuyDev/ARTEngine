


function init(self)
    self.anim_frame = 0
    self.culling_radius = 0
end

function update(self,dt)
    self.anim_frame = sum(self.anim_frame,24*dt)
    if self.anim_frame > 7 then
        self.anim_frame = 0
    end
    if input_kb_pressed(keyboard.ENTER) then
        entity_create("gamefiles/assets/code/test.lua",0,0,0)
    end
    if input_kb_down(keyboard.LEFT) then
        self.x = sub(self.x,3*dt)
    end
end

function draw(self)
    draw_model("prueba",self.x,self.y,self.z,0,0,0,1,1,1)
    draw_billboard_animated("prueba1",self.x-2,self.y,self.z,3,3,0.5,0,7,anim_frame)
end

function draw_screen(self)
    draw_sprite("prueba",0,0,0,1)
end