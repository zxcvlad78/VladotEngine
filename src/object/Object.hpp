#pragma once
#include <string>
#include <sol/sol.hpp>

class Object {
public:
    Object() = default;
    virtual ~Object() = default;
    virtual std::string get_class_name() const { return "Object"; }
};

#include "lua_binder/LuaBinder.hpp" 
#define REGISTER_LUA_TYPE(m_type, m_lambda) \
    static LuaBinder::Registrar m_type##_lua_reg(m_lambda);
