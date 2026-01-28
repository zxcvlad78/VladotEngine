-- Ссылка на игрока
local player = nil

-- Позиция спавна, которая будет различаться для каждого игрока
local spawn_positions = {
    vec2.new(200, 200), -- Позиция для игрока 1
    vec2.new(600, 500)  -- Позиция для игрока 2
}

event_system:on("ready", function()
    -- Создаем нового игрока из прототипа
    player = Sprite2D.new()
    player.texture = ResourceLoader.load_texture("textures/fredi.png")
    player.scale = vec2.new(0.5, 0.5)

    -- Определяем, чей это клиент, и устанавливаем соответствующую позицию спавна
    local my_id = Network.get_my_id()
    -- ID клиента начинается с 1. Мы используем его для индексации в таблице позиций.
    local spawn_index = my_id
    if spawn_index > #spawn_positions then spawn_index = 1 end -- На случай, если подключится больше 2 игроков

    --player.position = spawn_positions[spawn_index]

    -- ВАЖНО: Регистрируем объект в сети, чтобы он мог получать RPC-вызовы.
    Network.register_object(player) -- Temporarily disabled due to type casting error
    
    -- Добавляем игрока на сцену
    Scene:add_child(player)


    print("[Lua] Player spawned. My ID: " .. my_id .. " at " .. player.position.x .. ", " .. player.position.y)
end)

event_system:on("tick", function(delta)
    if not player then return end

    local velocity = vec2.new(0, 0)
    local current_speed = 300 -- Это значение можно взять из прототипа

    if Input.is_key_pressed(Key.LSHIFT) then
        current_speed = current_speed * 2.0
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

    -- Нормализуем вектор, если идет по диагонали, чтобы скорость не была выше
    if velocity.x ~= 0 and velocity.y ~= 0 then
        velocity = velocity * 0.7071 -- 1/sqrt(2)
    end

    -- Вычисляем новую позицию
    local new_pos = player.position + velocity * current_speed * delta

    -- Если позиция изменилась, отправляем RPC всем остальным
    if new_pos ~= player.position then
       player.position = new_pos
        -- Отправляем RPC на изменение позиции всем остальным игрокам (target_peer_id = -1)
       Network.send_rpc("SetPosition", {x = new_pos.x, y = new_pos.y}, -1, -1)
    end

    -- Отладка: Вывод текущей позиции
    --print(player, player.texture, player.position)
end)