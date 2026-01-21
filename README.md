# VladotEngine

A lightweight, modular 2D game engine built with C++20, focusing on high extensibility, Lua scripting, and a flexible modding system.


## Tech Stack

*   **Core:** C++20
*   **Graphics:** OpenGL 3.3 (Core Profile), GLFW, GLAD, GLM
*   **Scripting:** Lua 5.4 + [sol2](https://github.com)
*   **Serialization:** [nlohmann/json](https://github.com)
*   **Compression:** [miniz](https://github.com)

## 🎮 Modding Guide

Mods can be distributed as folders in the `mods/` directory. Each mod requires an `info.json` file:

```json
{
  "name": "ExampleMod",
  "version": "1.0.0",
  "author": "YourName",
  "dependencies": ["Core"] // can be empty
}
```

## Scripting Example (control.lua)
```lua
event_system:on("ready", function()
    local player = create_sprite("res/textures/player.png", vec2(400, 300))
end)
    
event_system:on("tick", function(delta)
    if Input.is_key_pressed(Key.W) then
        player.position.y = player.position.y - (200 * delta)
    end
end)
```

## Building form source
1. Clone the repository:
```bash
git clone https://github.com
```
2. Ensure you have a C++20 compatible compiler.

3. Build using CMake
```bash
mkdir build && cd build
cmake ..
make
```
