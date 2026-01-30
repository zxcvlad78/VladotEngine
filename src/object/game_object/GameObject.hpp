//GameObject.hpp

#pragma once
#include <glm/glm.hpp> 
#include <lua.hpp> 
#include "object/Object.hpp"
#include <nlohmann/json.hpp>
#include <object/ref_counted/RefCounted.hpp>

class GameObject : public Object {
private:
    std::vector<Ref<GameObject>> m_children;
    Ref<GameObject> m_parent;
public:
    GameObject() = default;
    virtual ~GameObject() {
        for (Ref<GameObject> p_child : m_children) {
            remove_child(p_child);
        }
    }

    void add_child(Ref<GameObject> p_game_object) {
        if (p_game_object) {
            m_children.push_back(p_game_object);
            p_game_object->set_parent(Ref<GameObject>(this));
            p_game_object->_enter_tree();
            p_game_object->_ready();
        }
    }
    Ref<GameObject> get_child(size_t index) {
        if (index < m_children.size()) return m_children[index];
        return nullptr;
    }
    std::vector<Ref<GameObject>> get_children() const { return m_children; }
    int get_child_count() const { return m_children.size(); }

    void remove_child(Ref<GameObject> p_child) {
        p_child->set_parent(nullptr);
        p_child->_exit_tree();
        m_children.erase(std::remove(m_children.begin(), m_children.end(), p_child), m_children.end());
    }

    std::vector<Ref<GameObject>> get_children_recursive() {
        std::vector<Ref<GameObject>> r_children;
        for (Ref<GameObject> p_child : get_children()) {
            r_children.push_back(p_child);
            for (auto n_children : p_child->get_children_recursive()) {
                r_children.push_back(n_children);
            }
        }
        return r_children;
    }

    void set_parent(Ref<GameObject> p_parent) { m_parent = p_parent; }
    Ref<GameObject> get_parent() const { return m_parent; }

    virtual void _enter_tree() {}
    virtual void _exit_tree() {}
    virtual void _ready() {}
    virtual void _process(float delta) {}
    virtual void _draw() {}

    std::string get_class_name() const override { return "GameObject"; }

    virtual void handle_rpc(const std::string& func_name, const nlohmann::json& args) {}

    static void bind_lua(lua_State* L);
};
