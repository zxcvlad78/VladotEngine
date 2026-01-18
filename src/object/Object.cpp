#include "Object.hpp"
#include <iostream>

// Регистрация базового класса Object
REGISTER_LUA_TYPE(Object, ([](sol::state& lua) {
    lua.new_usertype<Object>("Object",
        "get_class_name", &Object::get_class_name
    );
}))
