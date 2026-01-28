//GameObject.hpp

#pragma once
#include <glm/glm.hpp> 
#include <lua.hpp> 
#include "object/Object.hpp"
#include <nlohmann/json.hpp>

class GameObject : public Object {
public:
    GameObject() = default;
    virtual ~GameObject() override = default;

    std::string get_class_name() const override { return "GameObject"; }

    virtual void _process(float delta) {}
    virtual void _draw() {}
    virtual void handle_rpc(const std::string& func_name, const nlohmann::json& args) {}

    static void bind_lua(lua_State* L);
};
