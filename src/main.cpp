#define SOL_ALL_SAFETIES_ON 1
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sol/sol.hpp>
#include <iostream>
#include <chrono>

#include "object/resource/ResourceLoader.hpp"
#include "object/scene_tree/SceneTree.hpp"
#include "mod_loader/ModLoader.hpp"
#include "lua_binder/LuaBinder.hpp"
#include "virtual_fs/VirtualFS.hpp"

#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"

namespace Settings {
    const float FIXED_DELTA_TIME = 1.0f / 60.0f;
}

class Game : public Engine::IRegistry, public Engine::IGameplayAPI {
    Engine::EventSystem eventSystem;
public:
    void register_prototype(sol::table config) override { }
    Engine::EventSystem* get_event_system() { return &eventSystem; }
    void ready() { eventSystem.emit("ready"); }
    void tick(float delta) { eventSystem.emit("on_tick", delta); }
};

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vladot Engine 2026", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 1. Инициализация Lua и Биндингов
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);
    LuaBinder::bind_all(lua);

    // 2. Инициализация VFS и Ресурсов
    VirtualFS vfs;
    vfs.mount("./res", VirtualFS::FOLDER);
    ResourceLoader::initialize(&vfs);

    // 3. Загрузка Модов
    Game game;
    ModLoader modLoader;
    modLoader.scan_mods("./mods", &vfs);
    modLoader.load_data_stage(lua, &game, &vfs);
    modLoader.load_control_stage(lua, &game, game.get_event_system(), &vfs);

    game.ready();
    Ref<Sprite2D> sp = Ref<Sprite2D>(new Sprite2D()); 
    sp->set_texture(ResourceLoader::load<TextureResource>("icon.jpg")); 
    sp->set_shader(ResourceLoader::load<ShaderResource>("shaders/sprite.glsl"));

    SceneTree::get_singleton()->add_child(sp);

    auto last_time = std::chrono::high_resolution_clock::now();
    float accumulator = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        accumulator += std::min(delta, 0.25f);

        while (accumulator >= Settings::FIXED_DELTA_TIME) {
            game.tick(Settings::FIXED_DELTA_TIME);
            SceneTree::get_singleton()->update(Settings::FIXED_DELTA_TIME);
            accumulator -= Settings::FIXED_DELTA_TIME;
        }

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        SceneTree::get_singleton()->update(Settings::FIXED_DELTA_TIME);
        SceneTree::get_singleton()->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
