#pragma once
#include <glm/glm.hpp> 
#include <lua.hpp> 
#include "object/Object.hpp"

class GameObject : public Object {
public:
    GameObject() = default;
    virtual ~GameObject() override = default;

    std::string get_class_name() const override { return "GameObject"; }

    virtual void _process(float delta) {}
    virtual void _draw() {}

    static void bind_lua(lua_State* L);
};
