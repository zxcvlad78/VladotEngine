#pragma once
#include <string>
#include <lua.hpp>
#include <nlohmann/json.hpp> 
#include "vector"

struct lua_State;

class Object {
private:
    std::string m_name;
    std::vector<Object> m_components;
    
public:
    Object() { set_name(""); }
    Object(const std::string& p_name) {
        set_name(p_name);
    }
    virtual ~Object() = default;

    void set_name(const std::string& p_name) {
        if (p_name.empty()) {
            m_name = get_class_name();
            return;
        }
        m_name = p_name; }
    std::string get_name() const { return m_name; }

    virtual std::string get_class_name() const { return "Object"; }
    std::string to_string() const { return get_class_name() + "::" + m_name; }

    /**
     * @brief Handle remote procedure call.
     * @param func_name Name of the function to call
     * @param args Arguments for the function
     */
    virtual void handle_rpc(const std::string& func_name, const nlohmann::json& args) {}

    /**
     * @brief Unique network identifier.
     * -1 means the object is local and not synced across the network.
    */
    int network_id = -1;
};
