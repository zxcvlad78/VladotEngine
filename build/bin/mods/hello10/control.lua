-- control.lua

-- Подписываемся на сигнал готовности
event_system:on("ready", function()
    print("[Mod] Game is ready, spawning player...")

    local player = Sprite2D.new()
    
    -- Убедитесь, что пути верны для вашей VFS
    player:set_shader("../../res/shaders/sprite.glsl")
    player:set_texture("Wojak.png")
    
    player.position = vec2.new(400, 300)
    
    add_child(player)
end)

-- Логика обновления остается снаружи (подписка на события тика)
event_system:on("on_game_tick", function(delta)
    -- Здесь логика движения (если player объявлен глобально или захвачен)
end)
