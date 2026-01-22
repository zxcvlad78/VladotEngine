//LuaBinder.hpp

#pragma once
#include <lua.hpp>
#include <functional>

struct GLFWwindow; 

class LuaBinder {
public:
    static void bind_all(lua_State* L, GLFWwindow* window);
    static void add_to_registry(std::function<void(lua_State*)> func);
};
