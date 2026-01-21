-- globals.lua

-- COLORS
Color = {
    white      = {1.0, 1.0, 1.0, 1.0},
    black      = {0.0, 0.0, 0.0, 1.0},
    transparent= {0.0, 0.0, 0.0, 0.0},
    
    red        = {1.0, 0.0, 0.0, 1.0},
    green      = {0.0, 1.0, 0.0, 1.0},
    blue       = {0.0, 0.0, 1.0, 1.0},
    yellow     = {1.0, 1.0, 0.0, 1.0},
    magenta    = {1.0, 0.0, 1.0, 1.0},
    cyan       = {0.0, 1.0, 1.0, 1.0},
    
    orange     = {1.0, 0.65, 0.0, 1.0},
    purple     = {0.5, 0.0, 0.5, 1.0},
    pink       = {1.0, 0.75, 0.8, 1.0},
    gray       = {0.5, 0.5, 0.5, 1.0},
    dark_gray  = {0.2, 0.2, 0.2, 1.0},
    light_gray = {0.8, 0.8, 0.8, 1.0},
    brown      = {0.6, 0.3, 0.1, 1.0},
    
    skyblue    = {0.53, 0.81, 0.98, 1.0},
    mint       = {0.6, 1.0, 0.6, 1.0},
    gold       = {1.0, 0.84, 0.0, 1.0},
    crimson    = {0.86, 0.08, 0.24, 1.0}
}

function Color.from_hex(hex)
    local r = ((hex >> 16) & 0xFF) / 255.0
    local g = ((hex >> 8) & 0xFF) / 255.0
    local b = (hex & 0xFF) / 255.0
    return {r, g, b, 1.0}
end

function Color.alpha(color, a)
    return {color[1], color[2], color[3], a}
end

function Color.lerp(c1, c2, t)
    return {
        c1[1] + (c2[1] - c1[1]) * t,
        c1[2] + (c2[2] - c1[2]) * t,
        c1[3] + (c2[3] - c1[3]) * t,
        c1[4] + (c2[4] - c1[4]) * t
    }
end

-- END

---@class Object
local Object = {}
---@return string
function Object:get_class_name() end
---@return string
function Object:to_string() end

function Object.new() end

---@class RefCounted : Object
local RefCounted = {}

---@class Resource : Object
---@field virtual_path string
---@field rid integer
local Resource = {}
---@return string
function Resource:get_path() end
---@return integer
function Resource:get_rid() end

---@class TextureResource : Resource
local TextureResource = {}
---@return integer
function TextureResource:get_width() end
---@return integer
function TextureResource:get_height() end

---@class AudioResource : Resource
local AudioResource = {}


---@class GameObject : Object
local GameObject = {}

---@class GameObject2D : GameObject
---@field position vec2
---@field scale vec2
---@field rotation number
local GameObject2D = {}

---@class Sprite2D : GameObject2D
---@field texture TextureResource
---@field shader any
local Sprite2D = {}
---@return Sprite2D
function Sprite2D.new() end

---@class Input
Input = {}
---@param key_code integer
---@return boolean
function Input.is_key_pressed(key_code) end
---@param button_code integer
---@return boolean
function Input.is_mouse_pressed(button_code) end

---@class EventSystem
event_system = {}
---@param event_name string
---@param callback function
function event_system:on(event_name, callback) end
---@param event_name string
---@param ... any 
function event_system:emit(event_name, ...) end

---@type SceneTree
Scene = {}

---@class SceneTree : Object
local SceneTree = {}
---@param child GameObject
function SceneTree:add_child(child) end
---@return integer
function SceneTree:get_child_count() end
---@param index integer
---@return GameObject
function SceneTree:get_child(index) end

function OnTick(callback)
    if event_system then
        event_system:on("on_tick", callback)
    end
end

function OnReady(callback)
    if event_system then
        event_system:on("ready", callback)
    end
end

