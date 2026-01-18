#pragma once
#include "object/Object.hpp"
#include <glm/glm.hpp>

class GameObject : public Object {
public:
    GameObject() = default;
    std::string get_class_name() const override { return "GameObject"; }

    virtual void _process(float delta) {}
    virtual void _draw() {}
};
