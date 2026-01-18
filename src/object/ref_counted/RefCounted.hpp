#pragma once
#include "object/Object.hpp"
#include <memory>

/**
 * RefCounted - базовый класс для объектов с автоматическим управлением памятью.
 * В отличие от GameObject, которые управляются сценой, RefCounted объекты 
 * удаляются, когда на них не остается ссылок Ref<T>.
 */
class RefCounted : public Object {
public:
    RefCounted() = default;
    virtual ~RefCounted() override = default;

    virtual std::string get_class_name() const override { return "RefCounted"; }
};

/**
 * Ref<T> - аналог Godot Ref. 
 * Использует shared_ptr для реализации подсчета ссылок.
 */
template <typename T>
using Ref = std::shared_ptr<T>;

/**
 * Вспомогательная функция для создания ресурсов (аналог memnew в Godot)
 */
template <typename T, typename... Args>
Ref<T> create_ref(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
