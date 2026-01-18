#include "SceneTree.hpp"
#include "lua_binder/LuaBinder.hpp"
#include "object/game_object/GameObject.hpp"
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"

REGISTER_LUA_TYPE(SceneTree, [](sol::state& lua) {
    lua.set_function("add_child", [](sol::stack_object p_node) {
        
        // 1. Пробуем получить как прямой тип Ref<GameObject>
        auto as_go = p_node.as<sol::optional<Ref<GameObject>>>();
        if (as_go) {
            SceneTree::get_singleton()->add_child(*as_go);
            return;
        }

        // 2. РУЧНОЙ КАСТ: Если это Sprite2D, принудительно приводим к базе
        auto as_sprite = p_node.as<sol::optional<Ref<Sprite2D>>>();
        if (as_sprite) {
            // Создаем Ref на базовый класс, используя сырой указатель из наследника
            Ref<GameObject> forced_ref(static_cast<GameObject*>(as_sprite->get()));
            SceneTree::get_singleton()->add_child(forced_ref);
            return;
        }

        std::cerr << "[Critical] add_child: Cannot cast " 
                  << sol::type_name(p_node.lua_state(), p_node.get_type()) 
                  << " to GameObject!" << std::endl;
    });
})