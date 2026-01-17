#pragma once
#include <sol/sol.hpp>
#include <vector>
#include <functional>
#include <iostream>

class LuaBinder {
public:
    // Тип функции для регистрации класса в Lua-state
    using BindFunction = std::function<void(sol::state&)>;
    
    // Реестр всех функций биндинга
    static std::vector<BindFunction>& get_registry() {
        static std::vector<BindFunction> registry;
        return registry;
    }

    // Вспомогательный класс для автоматической регистрации биндинга
    struct Registrar {
        Registrar(BindFunction func) {
            get_registry().push_back(std::move(func));
        }
    };

    // Вызывает все зарегистрированные функции биндинга для данного Lua-state
    static void bind_all(sol::state& lua) {
        std::cout << "[LuaBinder] Executing all registered bindings..." << std::endl;
        for (const auto& bind_func : get_registry()) {
            bind_func(lua);
        }
    }
};
