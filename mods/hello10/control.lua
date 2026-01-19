local player = Sprite2D.new()

player:set_texture("icon.jpg")
player:set_shader("sprite.glsl")

player.position = vec2:new(400, 300)

local player2 = Sprite2D.new()

player2:set_texture("Wojak.png")
player2:set_shader("sprite.glsl")

player2.position = vec2:new(700, 300)

print("Created object of type: " .. player:get_class_name())

Scene:add_child(player)
Scene:add_child(player2)
