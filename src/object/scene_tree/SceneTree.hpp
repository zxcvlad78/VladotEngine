#pragma once
#include <vector>
#include "object/Object.hpp"
#include "object/game_object/GameObject.hpp"
#include "object/ref_counted/RefCounted.hpp" // ОБЯЗАТЕЛЬНО ЗДЕСЬ

class SceneTree : public Object {
public:
    static SceneTree* get_singleton() {
        static SceneTree instance;
        return &instance;
    }

    // Теперь Ref<GameObject> будет распознан как шаблон
    void add_child(Ref<GameObject> p_node) {
        if (p_node) {
            m_nodes.push_back(p_node);
        }
    }

    void update(float p_delta) {
        for (auto& node : m_nodes) {
            node->_process(p_delta);
        }
    }

    void render() {
        for (auto& node : m_nodes) {
            node->_draw();
        }
    }

    virtual std::string get_class_name() const override { return "SceneTree"; }

private:
    SceneTree() = default;
    std::vector<Ref<GameObject>> m_nodes;
};
