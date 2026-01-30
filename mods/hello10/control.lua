local player = Sprite2D.new()
local player2 = Sprite2D.new()

local speed = 200

event_system:on("ready", function()
    player.name = "Sprite2D_1"
    player.texture = ResourceLoader.load_texture("textures/icon.jpg")
    player.position = vec2:new(400, 300)


    player2.name = "Sprite2D_2"
    player2.texture = ResourceLoader.load_texture("textures/Wojak.png")
    player2.position = vec2:new(700, 300)
    player2.scale = vec2:new(2, 2)

    Scene:add_child(player)
    player:add_child(player2)
    
end)

event_system:on("tick", function(delta)
    if player then
        local pos = player.position
        pos.x = pos.x + (speed * delta)
        
        if pos.x > Window.get_width() then pos.x = 0 - player.texture:get_width() end
        
        player.position = pos
    end
end)