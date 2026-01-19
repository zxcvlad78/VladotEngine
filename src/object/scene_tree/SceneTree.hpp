#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "object/Object.hpp"
#include "object/game_object/GameObject.hpp"
#include "object/ref_counted/RefCounted.hpp"

class SceneTree : public Object {
public:
    static glm::mat4 current_projection;


    static SceneTree* get_singleton() {
        static SceneTree instance;
        return &instance;
    }


    void add_child(Ref<GameObject> p_node) {
        if (p_node) {
            m_nodes.push_back(p_node);
        }
    }

    std::vector<Ref<GameObject>> get_children() {
        return m_nodes;
    }

    void update(float p_delta) {
        for (auto& node : m_nodes) {
            if (node) node->_process(p_delta);
        }
    }

    void render() {
        for (auto& node : m_nodes) {
            if (node) {
                node->_draw(); 
            }
        }
    }

    virtual std::string get_class_name() const override { return "SceneTree"; }

private:
    SceneTree() = default;
    std::vector<Ref<GameObject>> m_nodes;
};
