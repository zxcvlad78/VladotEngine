local player = Sprite2D.new()
local player2 = Sprite2D.new()

local speed = 200

event_system:on("ready", function()
    player.texture = ResourceLoader.load_texture("icon.jpg")
    player.shader = ResourceLoader.load_shader("sprite.glsl")

    player.position = vec2:new(400, 300)


    player2.texture = ResourceLoader.load_texture("Wojak.png")
    player2.shader = ResourceLoader.load_shader("sprite.glsl")

    player2.position = vec2:new(700, 300)

    print("Created object of type: " .. player:get_class_name())

    Scene:add_child(player)
    Scene:add_child(player2)
end)

event_system:on("on_tick", function(delta)
    if player then
        print("Moving player")
        local pos = player.position
        pos.x = pos.x + (speed * delta)
        
        if pos.x > 1280 + player.texture.width then pos.x = 0 end
        
        player.position = pos
    end
end)