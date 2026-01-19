#include "lua_binder/LuaBinder.hpp"
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "engine_api/EngineAPI.hpp"
#include "object/Object.hpp"
#include "object/resource/ResourceLoader.hpp"
#include "object/game_object/game_object_2d/sprite_2d/Sprite2D.hpp"
#include "object/scene_tree/SceneTree.hpp"

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

void LuaBinder::bind_all(lua_State* L) {
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
        
        "set_texture", [](Sprite2D& self, const std::string& path) {
            auto res = ResourceLoader::load<TextureResource>(path);
            if (res) self.set_texture(res);
        },
        "set_shader", [](Sprite2D& self, const std::string& path) {
            auto res = ResourceLoader::load<ShaderResource>(path);
            if (res) self.set_shader(res);
        }
    );

    lua.new_usertype<TextureResource>("TextureResource",
        sol::base_classes, sol::bases<Resource, RefCounted, Object>(),
        "width",  sol::readonly(&TextureResource::get_width),
        "height", sol::readonly(&TextureResource::get_height)
    );

    auto resource_loader = lua.create_named_table("ResourceLoader");
    resource_loader["load_texture"] = [](const std::string& path) {
        return ResourceLoader::load<TextureResource>(path);
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
}

