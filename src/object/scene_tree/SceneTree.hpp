//SceneTree.hpp

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "object/game_object/GameObject.hpp"
#include "object/ref_counted/RefCounted.hpp"

class SceneTree : public GameObject {
public:
    static glm::mat4 current_projection;


    static SceneTree* get_singleton() {
        static SceneTree instance;
        return &instance;
    }

    void _process(float p_delta) {
        for (auto& child : get_children()) {
            if (child) child->_process(p_delta);
        }
    }

    void render() {
        for (auto& child : get_children()) {
            if (child) {
                child->_draw(); 
            }
        }
    }

    virtual std::string get_class_name() const override { return "SceneTree"; }

private:
    SceneTree() = default;
};
