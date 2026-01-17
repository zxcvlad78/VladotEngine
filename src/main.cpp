#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <GLFW/glfw3.h>
#include "mod_loader/ModLoader.hpp"
#include <map>
#include <iostream>
#include "lua_binder/LuaBinder.hpp"

namespace Settings {
    const int TARGET_TICK_RATE = 60; 
    const int PHYSICS_TICK_RATE = 60; 
    const float FIXED_DELTA_TIME = 1.0f / static_cast<float>(TARGET_TICK_RATE);
}

class Game : public Engine::IRegistry, public Engine::IGameplayAPI {
    Engine::EventSystem eventSystem;

public:
    Game() = default;
    ~Game() = default;

    void register_prototype(sol::table config) override { /* Registration logic */ }

    Engine::EventSystem* get_event_system() { return &eventSystem; }

    void tick(float delta) {
        eventSystem.emit("on_tick", delta);
    }
    
    void physics_tick(float delta) {
         eventSystem.emit("on_physics_tick", delta);
    }
};


int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vladot Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
    LuaBinder::bind_all(lua);

    Game game;

    VirtualFS vfs;
    vfs.mount_recursive("./mods"); 
    
    ModLoader modLoader;

    modLoader.scan_mods("./mods");

    modLoader.load_data_stage(lua, &game, &vfs);
    modLoader.load_control_stage(lua, &game, game.get_event_system(), &vfs);
    
    std::cout << "--- Hello from VladotEngine ---" << std::endl;
    
    auto last_time = std::chrono::high_resolution_clock::now();
    float time_accumulator = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        if (delta_time > 0.25f) {
            delta_time = 0.25f;
        }
        time_accumulator += delta_time;

        while (time_accumulator >= Settings::FIXED_DELTA_TIME) {
            game.tick(Settings::FIXED_DELTA_TIME); 
            game.physics_tick(Settings::FIXED_DELTA_TIME); 
            
            game.get_event_system()->emit("on_game_tick", Settings::FIXED_DELTA_TIME);

            time_accumulator -= Settings::FIXED_DELTA_TIME;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float interpolation_alpha = time_accumulator / Settings::FIXED_DELTA_TIME;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
