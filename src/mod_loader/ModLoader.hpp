#pragma once
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <set>
#include "virtual_fs/VirtualFS.hpp"
#include <nlohmann/json.hpp> 
#include "engine_api/EngineAPI.hpp"

class ModLoader {
    using json = nlohmann::json;

    struct ModEntry {
        std::string name;
        std::string version;
        std::string author;
        std::vector<std::string> dependencies;
        std::filesystem::path path;
        std::string data_script_name = "data.lua";
        std::string control_script_name = "control.lua";
        bool enabled = true;
    };

    std::map<std::string, ModEntry> availableMods;
    std::vector<ModEntry*> sortedLoadOrder;

    void resolve_dependencies();
    
public:
    ModLoader() = default;
    void scan_mods(const std::filesystem::path& folder, VirtualFS* vfs);
    void load_data_stage(sol::state& lua, Engine::IRegistry* registry, VirtualFS* vfs); 
    void load_control_stage(sol::state& lua, Engine::EventSystem* events, VirtualFS* vfs);
};
