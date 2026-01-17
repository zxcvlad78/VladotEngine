#pragma once
#include <string>
#include <sol/sol.hpp>

namespace Engine {
    struct EntityPrototype {
        std::string name;
        float health;
        float speed;
        std::string texture;
    };

    class IRegistry {
    public:
        virtual ~IRegistry() = default;
        virtual void register_prototype(sol::table config) = 0;
    };
}
