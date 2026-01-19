#pragma once
#include <lua.hpp>
#include <functional>

class LuaBinder {
public:
    static void bind_all(lua_State* L);
    static void add_to_registry(std::function<void(lua_State*)> func);
};
