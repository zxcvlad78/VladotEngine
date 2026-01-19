-- ==========================================
-- ОСНОВНЫЕ ГЛОБАЛЬНЫЕ СИСТЕМЫ
-- ==========================================

-- Синглтоны (сокращения для удобства)
SceneTreeSingleton = SceneTree.get_singleton()

-- Математические обертки (если vec2 был привязан как класс)
-- Позволяет писать v = vec2(10, 20) вместо vec2.new(10, 20)
setmetatable(_G, {
    __index = function(t, key)
        if key == "vec2" then
            return function(x, y) return vec2.new(x or 0, y or 0) end
        end
    end
})

-- ==========================================
-- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (API для модов)
-- ==========================================

--- Создает спрайт и сразу добавляет его в мир (опционально)
--- @param texture_path string Путь к текстуре
--- @param pos table (vec2) Позиция
function spawn_sprite(texture_path, pos)
    local sprite = Sprite2D.new()
    local tex = ResourceLoader.load_texture(texture_path)
    
    if tex then
        sprite:set_texture(tex)
    else
        print("[Lua Error] Failed to load texture: " .. texture_path)
    end
    
    if pos then
        sprite:set_position(pos)
    end
    
    World:add_child(sprite)
    return sprite
end

--- Глобальная таблица данных (то, что заполняет data_extend)
data = {
    prototypes = {}
}

--- Функция для регистрации прототипов (используется в data.lua модов)
function data_extend(table_list)
    for _, item in ipairs(table_list) do
        if item.type and item.name then
            data.prototypes[item.name] = item
            print("[Data] Registered prototype: " .. item.type .. " -> " .. item.name)
        end
    end
end

-- ==========================================
-- СИСТЕМА СОБЫТИЙ (Улучшенная)
-- ==========================================
local _listeners = {}

--- Подписка на событие
function subscribe(event_name, callback)
    if not _listeners[event_name] then
        _listeners[event_name] = {}
    end
    table.insert(_listeners[event_name], callback)
end

--- Вызов события из C++ (движок дергает эту функцию)
function emit_event(event_name, ...)
    local callbacks = _listeners[event_name]
    if callbacks then
        for _, cb in ipairs(callbacks) do
            local status, err = pcall(cb, ...)
            if not status then
                print("[Event Error] " .. event_name .. ": " .. tostring(err))
            end
        end
    end
end

print("[Globals] Base API initialized.")
