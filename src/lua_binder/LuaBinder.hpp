#pragma once
#include <sol/sol.hpp>
#include <vector>
#include <functional>

class LuaBinder {
public:
    using BindFunction = std::function<void(sol::state&)>;
    static std::vector<BindFunction>& get_registry();

    struct Registrar {
        Registrar(BindFunction func);
    };

    static void bind_all(sol::state& lua);
private:
    static void register_core_types(sol::state& lua);
};

#define REGISTER_LUA_TYPE(m_type, ...) \
    namespace { \
        struct m_type##Registrar { \
            m_type##Registrar() { \
                LuaBinder::get_registry().push_back(__VA_ARGS__); \
            } \
        }; \
        static m_type##Registrar m_type##_reg_instance; \
    }