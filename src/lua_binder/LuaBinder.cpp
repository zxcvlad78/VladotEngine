#include "lua_binder/LuaBinder.hpp"
#include "object/Object.hpp"
#include "object/game_object/GameObject.hpp"
#include "object/game_object/game_object_2d/GameObject2D.hpp"
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/scene_tree/SceneTree.hpp"
#include "object/resource/ResourceLoader.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

std::vector<LuaBinder::BindFunction>& LuaBinder::get_registry() {
    static std::vector<BindFunction> registry;
    return registry;
}

LuaBinder::Registrar::Registrar(BindFunction func) {
    LuaBinder::get_registry().push_back(std::move(func));
}

void LuaBinder::bind_all(sol::state& lua) {
    std::cout << "[LuaBinder] Initializing Core 2026 (Hard-bind mode)..." << std::endl;
    
    // 1. Векторы
    lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x, "y", &glm::vec2::y
    );

    // 2. Регистрация иерархии типов (в ОДНОМ месте)
    lua.new_usertype<Object>("Object");

    lua.new_usertype<GameObject>("GameObject", 
        sol::base_classes, sol::bases<Object>()
    );

    lua.new_usertype<GameObject2D>("GameObject2D", 
        sol::base_classes, sol::bases<GameObject, Object>(),
        "position", sol::property(&GameObject2D::GetPosition, &GameObject2D::SetPosition),
        "scale",    sol::property(&GameObject2D::GetScale,    &GameObject2D::SetScale),
        "rotation", sol::property(&GameObject2D::GetRotation, &GameObject2D::SetRotation)
    );

    // 3. Регистрация Sprite2D (вынесено сюда из Sprite2D.cpp для гарантии каста)
    lua.new_usertype<Sprite2D>("Sprite2D",
        "new", sol::factories([]() { return std::make_shared<Sprite2D>(); }),
        sol::call_constructor, sol::factories([]() { return std::make_shared<Sprite2D>(); }),
        sol::base_classes, sol::bases<GameObject2D, GameObject, Object>(),
        
        "set_texture", [](Sprite2D& self, const std::string& path) {
            auto res = ResourceLoader::load<TextureResource>(path);
            if (res) self.set_texture(res);
        },
        "set_shader", [](Sprite2D& self, const std::string& path) {
            auto res = ResourceLoader::load<ShaderResource>(path);
            if (res) self.set_shader(res);
        }
    );

    // 4. Тот самый add_child. Принимаем shared_ptr<GameObject>
    lua["add_child"] = [](std::shared_ptr<GameObject> obj) {
        if (!obj) {
            std::cerr << "[Lua Error] add_child: nil object!" << std::endl;
            return;
        }
        if (SceneTree::get_singleton() && SceneTree::get_singleton()) {
            SceneTree::get_singleton()->add_child(obj);
        }
    };

    // 5. Вызов остальных регистраторов (если есть)
    for (const auto& bind_func : get_registry()) {
        bind_func(lua);
    }
}
