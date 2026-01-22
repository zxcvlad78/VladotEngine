//LuaBinder.cpp

#include "lua_binder/LuaBinder.hpp"
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <filesystem>

#include "engine_api/EngineAPI.hpp"
#include "object/Object.hpp"
#include "object/resource/ResourceLoader.hpp"
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/scene_tree/SceneTree.hpp"
#include "networking/network/Network.hpp"

#include <GLFW/glfw3.h>

template<typename Base>
Ref<Base> cast_from_lua(sol::object obj) {
    if (obj.is<Ref<Base>>()) {
        return obj.as<Ref<Base>>();
    }

    if (obj.is<sol::userdata>()) {
        sol::userdata ud = obj;
        try {
            return obj.as<Ref<Base>>();
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

void LuaBinder::bind_all(lua_State* L, GLFWwindow* window) {
    sol::state_view lua(L);

    std::cout << "[C++] Binding lua" << std::endl;

    lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y,
        sol::meta_function::addition,       [](const glm::vec2& a, const glm::vec2& b) { return a + b; },
        sol::meta_function::subtraction,    [](const glm::vec2& a, const glm::vec2& b) { return a - b; },
        sol::meta_function::multiplication, [](const glm::vec2& a, float b) { return a * b; },
        sol::meta_function::to_string,      [](const glm::vec2& v) { 
            return "vec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; 
        }
    );

    auto net_table = lua.create_named_table("Network");
    net_table["start_server"] = [](int port) { return Network::get().start_server(port); };
    net_table["connect"] = [](std::string ip, int port) { return Network::get().connect(ip, port); };
    net_table["get_my_id"] = []() { return Network::get().get_my_peer_id(); };
    net_table["get_last_sender_id"] = []() { return Network::get().get_last_sender_id(); };
    
    net_table["send_rpc"] = [](std::string func_name, sol::table args_table, 
                           sol::optional<int> target_obj_id, 
                           sol::optional<int> target_peer_id) {
        
        nlohmann::json j_args;
        for (auto const& [key, val] : args_table) {
            if (key.is<std::string>()) {
                std::string k_str = key.as<std::string>();
                if (val.is<std::string>()) {
                    j_args[k_str] = val.as<std::string>();
                } else if (val.is<float>()) {
                    j_args[k_str] = val.as<float>();
                } else if (val.is<int>()) {
                    j_args[k_str] = val.as<int>();
                }
            }
        }

        int obj_id_val = target_obj_id.value_or(-1);
        int peer_id_val = target_peer_id.value_or(-1);

        Network::get().send_rpc(func_name, j_args, obj_id_val, peer_id_val);
    };
    
    net_table["generate_next_object_id"] = []() {
        if (Network::get().get_my_peer_id() == 0) {
            return Network::get().generate_next_object_id();
        }
        return -1;
    };


    net_table["find_object_by_id"] = [](int id) -> Ref<Object> {
        return Network::get().get_object_by_id(id);
    };

    Network::get().set_rpc_handler([L](std::string name, nlohmann::json args_json, int obj_id, int sender_id) {
        sol::state_view lua(L);
        if (lua[name].valid()) {
           lua[name](args_json.dump(), obj_id, sender_id); 
        }
    });

    net_table["register_object"] = [](Ref<Object> obj) {
        Network::get().register_object(obj);
    };

    //-----

    lua.new_usertype<Object>("Object",
        "new", sol::factories([]() { return Ref<Object>(new Object()); } ),
        sol::call_constructor, sol::factories([]() { return Ref<Object>(new Object()); }),

        "get_class_name", [](Object& self) {
            return self.get_class_name();
        },

        "to_string", &Object::to_string
    );

    lua.new_usertype<GameObject>("GameObject", 
        "new", sol::factories([]() { return Ref<GameObject>(new GameObject()); } ),
        sol::call_constructor, sol::factories([]() { return Ref<GameObject>(new GameObject()); }),
        sol::base_classes, sol::bases<Object>()
    );


    lua.new_usertype<GameObject2D>("GameObject2D",
        "new", sol::factories([]() { return Ref<GameObject2D>(new GameObject2D()); } ),
        sol::call_constructor, sol::factories([]() { return Ref<GameObject2D>(new GameObject2D()); }),
        sol::base_classes, sol::bases<GameObject, Object>(),

        "position", sol::property(&GameObject2D::GetPosition, &GameObject2D::SetPosition),
        "scale",    sol::property(&GameObject2D::GetScale,    &GameObject2D::SetScale),
        "rotation", sol::property(&GameObject2D::GetRotation, &GameObject2D::SetRotation)
    );

    lua.new_usertype<Sprite2D>("Sprite2D",
        "new", sol::factories([]() { return Ref<Sprite2D>(new Sprite2D()); }),
        sol::call_constructor, sol::factories([]() { return Ref<Sprite2D>(new Sprite2D()); }),
        sol::base_classes, sol::bases<GameObject2D, GameObject, Object>(),
        
        "texture", sol::property(&Sprite2D::get_texture, &Sprite2D::set_texture),
        "shader", sol::property(&Sprite2D::get_shader, &Sprite2D::set_shader)
    );

    lua.new_usertype<Resource>("Resource",
        sol::base_classes, sol::bases<RefCounted, Object>(),
        "get_path", [](Resource& self) {
            return self.get_path();
        },
        "get_rid", [](Resource& self) {
            return self.get_rid();
        }
    );

    lua.new_usertype<TextureResource>("TextureResource",
        sol::base_classes, sol::bases<Resource, RefCounted, Object>(),
        "get_width",  [](TextureResource& self) {
            return self.get_width();
        },
        "get_height",  [](TextureResource& self) {
            return self.get_height();
        }
    );

    auto resource_loader = lua.create_named_table("ResourceLoader");
    resource_loader["load_texture"] = [](const std::string& path) {
        return ResourceLoader::load<TextureResource>(path);
    };
    resource_loader["load_shader"] = [](const std::string& path) {
        return ResourceLoader::load<ShaderResource>(path);
    };


    lua.new_usertype<SceneTree>("SceneTree",
        sol::base_classes, sol::bases<Object>(),

        "get_children", &SceneTree::get_children,
        
        "get_child_count", &SceneTree::get_child_count,
        
        "get_child", &SceneTree::get_child,

        "add_child", [](SceneTree& self, GameObject* obj) {
            if (obj) {
                self.add_child(Ref<GameObject>(obj, [](GameObject*){}));
            }
        },
        "get_singleton", [](SceneTree& self) {
            return self.get_singleton();
        }
    );

    
    lua["Scene"] = SceneTree::get_singleton(); 

    auto input_bind = lua.create_named_table("Input");

    input_bind["is_key_pressed"] = [window](int key_code) {
        return glfwGetKey(window, key_code) == GLFW_PRESS;
    };

    input_bind["is_mouse_pressed"] = [window](int button) {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    };

    auto keys = lua.create_named_table("Key");

    for (char c = 'A'; c <= 'Z'; ++c) {
        keys[std::string(1, c)] = (int)c; 
    }

    for (char c = '0'; c <= '9'; ++c) {
        keys[std::string(1, c)] = (int)c;
    }

    for (int i = 1; i <= 12; ++i) {
        keys["F" + std::to_string(i)] = GLFW_KEY_F1 + (i - 1);
    }

    keys["SPACE"]    = GLFW_KEY_SPACE;
    keys["ENTER"]    = GLFW_KEY_ENTER;
    keys["ESCAPE"]   = GLFW_KEY_ESCAPE;
    keys["BACKSPACE"] = GLFW_KEY_BACKSPACE;
    keys["TAB"]      = GLFW_KEY_TAB;
    keys["LEFT"]     = GLFW_KEY_LEFT;
    keys["RIGHT"]    = GLFW_KEY_RIGHT;
    keys["UP"]       = GLFW_KEY_UP;
    keys["DOWN"]     = GLFW_KEY_DOWN;
    keys["LSHIFT"]   = GLFW_KEY_LEFT_SHIFT;
    keys["RSHIFT"]   = GLFW_KEY_RIGHT_SHIFT;
    keys["LCTRL"]    = GLFW_KEY_LEFT_CONTROL;
    keys["RCTRL"]    = GLFW_KEY_RIGHT_CONTROL;
    keys["LALT"]     = GLFW_KEY_LEFT_ALT;
    keys["RALT"]     = GLFW_KEY_RIGHT_ALT;
    keys["INSERT"]   = GLFW_KEY_INSERT;
    keys["DELETE"]   = GLFW_KEY_DELETE;
    keys["PAGE_UP"]  = GLFW_KEY_PAGE_UP;
    keys["PAGE_DOWN"] = GLFW_KEY_PAGE_DOWN;
    keys["HOME"]     = GLFW_KEY_HOME;
    keys["END"]      = GLFW_KEY_END;
    keys["CAPS_LOCK"] = GLFW_KEY_CAPS_LOCK;
    keys["PRINT_SCREEN"] = GLFW_KEY_PRINT_SCREEN;
    keys["PAUSE"]    = GLFW_KEY_PAUSE;

    keys["MOUSE_1"]  = GLFW_MOUSE_BUTTON_1; // LMB
    keys["MOUSE_2"]  = GLFW_MOUSE_BUTTON_2; // RMB
    keys["MOUSE_3"]  = GLFW_MOUSE_BUTTON_3; // MWHEEL
}

