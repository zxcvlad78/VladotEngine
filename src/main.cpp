#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sol/sol.hpp>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "virtual_fs/VirtualFS.hpp"
#include "object/resource/ResourceLoader.hpp"
#include "object/scene_tree/SceneTree.hpp"
#include "mod_loader/ModLoader.hpp"
#include "lua_binder/LuaBinder.hpp"
#include "engine_api/EngineAPI.hpp"

class Game : public Engine::IRegistry {
public:
    void register_prototype(sol::table prototype) override {
        std::cout << "[Game] Prototype registered" << std::endl;
    }
};

namespace Settings {
    const float FIXED_DELTA_TIME = 1.0f / 60.0f;
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
}

int main() {
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "VladotEngine 2026", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::string, sol::lib::math);
    LuaBinder::bind_all(lua.lua_state());

    VirtualFS vfs;
    vfs.mount("./res", VirtualFS::FOLDER);
    ResourceLoader::initialize(&vfs);

    
    Game gameInstance;
    Engine::EventSystem eventSystem;
    
    ModLoader modLoader;
    modLoader.scan_mods("./mods", &vfs);
    modLoader.load_data_stage(lua, &gameInstance, &vfs);
    modLoader.load_control_stage(lua, &eventSystem, &vfs);
    
    eventSystem.emit("ready");
    
    auto last_time = std::chrono::high_resolution_clock::now();
    float accumulator = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        accumulator += std::min(delta, 0.25f);

        while (accumulator >= Settings::FIXED_DELTA_TIME) {
            // Исправлено: событие on_tick для Lua
            eventSystem.emit("on_tick", Settings::FIXED_DELTA_TIME);
            
            SceneTree::get_singleton()->update(Settings::FIXED_DELTA_TIME);
            accumulator -= Settings::FIXED_DELTA_TIME;
        }

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        SceneTree::get_singleton()->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
