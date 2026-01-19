#pragma once
#include "object/Object.hpp"
#include <memory>

class RefCounted : public Object {
public:
    RefCounted() = default;
    virtual ~RefCounted() override = default;

    virtual std::string get_class_name() const override { return "RefCounted"; }
};


template <typename T>
using Ref = std::shared_ptr<T>;


template <typename T, typename... Args>
Ref<T> create_ref(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
