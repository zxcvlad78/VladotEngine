#pragma once
#include <sol/sol.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace Engine {

    class IRegistry {
    public:
        virtual ~IRegistry() = default;
        // Используем sol::table для автоматического управления ссылкой на таблицу
        virtual void register_prototype(sol::table prototype) = 0;
    };

    class IGameplayAPI {
    public:
        virtual ~IGameplayAPI() = default;
    };

    class EventSystem {
        public:
            std::map<std::string, std::vector<sol::function>> listeners;

            // Метод 'on' для Lua
            void on(const std::string& event_name, sol::function callback) {
                listeners[event_name].push_back(std::move(callback));
            }

            template<typename... Args>
            void emit(const std::string& event_name, Args&&... args) {
                auto it = listeners.find(event_name);
                if (it != listeners.end()) {
                    for (auto& func : it->second) {
                        auto result = func(std::forward<Args>(args)...);
                        if (!result.valid()) {
                            sol::error err = result;
                            std::cerr << "[Event Error] " << event_name << ": " << err.what() << std::endl;
                        }
                    }
                }
            }
        };
}
