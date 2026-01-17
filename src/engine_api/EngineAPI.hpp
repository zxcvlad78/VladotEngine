#pragma once
#include <string>
#include <sol/sol.hpp>
#include <iostream>

namespace Engine {
    class IRegistry {
    public:
        virtual ~IRegistry() = default;
        virtual void register_prototype(sol::table config) = 0;
    };
    
    class IGameplayAPI {
    public:
        virtual ~IGameplayAPI() = default;
    };

    class EventSystem {
    public:
        // Key: Event Name (e.g., "on_player_click", "on_entity_die")
        // Value: List of Lua functions (callbacks)
        std::map<std::string, std::vector<sol::function>> listeners;

        void subscribe(const std::string& eventName, sol::function callback) {
            listeners[eventName].push_back(std::move(callback));
        }

        template<typename... Args>
        void emit(const std::string& eventName, Args&&... args) {
            if (listeners.count(eventName)) {
                for (auto& callback : listeners[eventName]) {
                    callback(std::forward<Args>(args)...);
                }
            }
        }
    };
}
