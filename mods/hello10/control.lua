local player = Sprite2D.new()
local children = Scene:get_children()

player:set_texture("icon.jpg")

player.position = vec2:new(400, 300)
player.scale = vec2:new(2.0, 2.0)
player.rotation = 45.0

print("Created object of type: " .. player:get_class_name())

Scene:add_child(player)

--print(children)