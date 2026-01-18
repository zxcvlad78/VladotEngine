-- ==========================================
-- VladotEngine: Global Definitions
-- ==========================================

-- 1. Упрощение создания векторов
-- Теперь можно писать просто: pos = vec(10, 20)
function vec(x, y)
    return vec2.new(x or 0, y or 0)
end

-- 2. Коды клавиш (стандарт GLFW)
-- Соответствуют значениям из GLFW/glfw3.h
Input = {
    KEY_W = 87,
    KEY_A = 65,
    KEY_S = 83,
    KEY_D = 68,
    KEY_SPACE = 32,
    KEY_ESCAPE = 256,
    KEY_LEFT  = 263,
    KEY_RIGHT = 262,
    KEY_UP    = 265,
    KEY_DOWN  = 264,
}

-- Заглушка для проверки нажатий (если C++ еще не пробросил Input.is_key_pressed)
if not Input.is_key_pressed then
    Input.is_key_pressed = function(key) 
        -- Эта функция будет переопределена из C++
        return false 
    end
end

-- 3. Константы экрана (соответствуют Settings в main.cpp)
Screen = {
    WIDTH = 800,
    HEIGHT = 600,
    CENTER = vec(400, 300)
}

-- 4. Математические помощники
Math = {
    lerp = function(a, b, t) return a + (b - a) * t end,
    clamp = function(v, min, max) return math.max(min, math.min(max, v)) end
}

-- 5. Системные события
-- Если event_system не проброшен, создаем пустой объект во избежание вылетов
if not event_system then
    event_system = {
        on = function(self, name, func) 
            print("[Warning] EventSystem not found in C++ for event: " .. name) 
        end
    }
end

print("[Globals] Base definitions loaded.")
