local player = Sprite2D.new()
local speed = 300
local run_multiplier = 2.0

event_system:on("ready", function()
    player.texture = ResourceLoader.load_texture("textures/fredi.png")
    player.position = vec2.new(400, 300)
    
    Scene:add_child(player)
end)

event_system:on("on_tick", function(delta)
    if not player then return end

    local velocity = vec2.new(0, 0)
    local current_speed = speed

    if Input.is_key_pressed(Key.LSHIFT) then
        current_speed = speed * run_multiplier
    end

    if Input.is_key_pressed(Key.W) or Input.is_key_pressed(Key.UP) then
        velocity.y = velocity.y - 1
    end
    if Input.is_key_pressed(Key.S) or Input.is_key_pressed(Key.DOWN) then
        velocity.y = velocity.y + 1
    end
    if Input.is_key_pressed(Key.A) or Input.is_key_pressed(Key.LEFT) then
        velocity.x = velocity.x - 1
    end
    if Input.is_key_pressed(Key.D) or Input.is_key_pressed(Key.RIGHT) then
        velocity.x = velocity.x + 1
    end

    local pos = player.position
    
    pos.x = pos.x + (velocity.x * current_speed * delta)
    pos.y = pos.y + (velocity.y * current_speed * delta)

    -- Простая проверка границ экрана (используя наше окно)
    local win_w = Window.get_width()
    local win_h = Window.get_height()

    if pos.x < 0 then pos.x = 0 end
    if pos.y < 0 then pos.y = 0 end
    if pos.x > win_w then pos.x = win_w end
    if pos.y > win_h then pos.y = win_h end

    player.position = pos
end)
