#pragma once
#include <string>
#include <lua.hpp>

struct lua_State;

class Object {
public:
    Object() = default;
    virtual ~Object() = default;
    std::string to_string() const { return get_class_name(); }
    virtual std::string get_class_name() const { return "Object"; };
    
    /**
     * @brief Unique network identifier.
     * -1 means the object is local and not synced across the network.
    */
    int network_id = -1;
};
