// ModLoader.hpp
#include <filesystem>
#include <vector>
#include "engine_api/EngineAPI.hpp"

class ModLoader {
    struct ModEntry {
        std::string name;
        std::filesystem::path path;
        std::vector<std::string> dependencies;
    };

    std::vector<ModEntry> _active_mods;
    void resolve_dependencies(); // Топологическая сортировка

public:
    void scan_mods(const std::filesystem::path& folder);
    void load_data_stage(sol::state& lua); // Фаза регистрации данных
    void load_control_stage(sol::state& lua); // Фаза логики
};
