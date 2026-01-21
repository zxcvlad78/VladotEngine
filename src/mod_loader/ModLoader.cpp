#include "ModLoader.hpp"
#include <set>
#include <algorithm>


void ModLoader::scan_mods(const std::filesystem::path& folder, VirtualFS* vfs) {
    if (!std::filesystem::exists(folder)) {
        std::cerr << "[ModLoader] Folder not found: " << folder << std::endl;
        return;
    }

    for (auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_directory()) {
            std::filesystem::path infoPath = entry.path() / "info.json";
            
            if (std::filesystem::exists(infoPath)) {
                std::ifstream f(infoPath);
                try {
                    nlohmann::json data = nlohmann::json::parse(f);
                    ModEntry m;
                    m.name = data["name"];
                    m.version = data["version"];
                    m.path = entry.path();
                    
                    if (data.contains("dependencies")) {
                        m.dependencies = data["dependencies"].get<std::vector<std::string>>();
                    }
                    
                    availableMods[m.name] = m;
                    
                    vfs->mount(entry.path().string(), VirtualFS::FOLDER);
                    
                    std::cout << "[ModLoader] Found Mod: " << m.name << " v" << m.version << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[ModLoader] Error parsing info.json in " << entry.path() << ": " << e.what() << std::endl;
                }
            }
        }
    }

    resolve_dependencies();
}

void ModLoader::resolve_dependencies() {
    sortedLoadOrder.clear();
    std::set<std::string> visited;
    std::set<std::string> visiting;

    std::function<void(const std::string&)> dfs = [&](const std::string& name) {
        if (visiting.count(name)) {
            std::cerr << "[ModLoader] КРИТИЧЕСКАЯ ОШИБКА: Обнаружена циклическая зависимость в моде " << name << std::endl;
            return;
        }
        if (visited.count(name)) return;

        visiting.insert(name);

        auto it = availableMods.find(name);
        if (it != availableMods.end()) {
            for (const auto& depName : it->second.dependencies) {
                dfs(depName);
            }
            visited.insert(name);
            sortedLoadOrder.push_back(&it->second);
        }

        visiting.erase(name);
    };

    for (auto const& [name, mod] : availableMods) {
        dfs(name);
    }
}

void ModLoader::load_data_stage(sol::state& lua, Engine::IRegistry* registry, VirtualFS* vfs) {
    auto data_table = lua.create_named_table("data");
    data_table["extend"] = [registry](sol::table config) { 
        if (registry) registry->register_prototype(config); 
    };

    for (auto* mod : sortedLoadOrder) {
        std::filesystem::path scriptPath = mod->path / "data.lua";
        if (std::filesystem::exists(scriptPath)) {
            std::cout << "[ModLoader] Loading data.lua for: " << mod->name << std::endl;
            auto result = lua.safe_script_file(scriptPath.string(), sol::script_default_on_error);
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua Error Data Stage] " << mod->name << ": " << err.what() << std::endl;
            }
        }
    }
}

void ModLoader::load_control_stage(sol::state& lua, Engine::EventSystem* events, VirtualFS* vfs) {
    auto events_table = lua.create_table();
    events_table["on"] = [events](sol::table self, const std::string& eventName, sol::function callback) {
        events->on(eventName, std::move(callback));
    };
    lua["event_system"] = events_table;

    for (auto* mod : sortedLoadOrder) {
        std::filesystem::path controlPath = mod->path / "control.lua";
        
        if (std::filesystem::exists(controlPath)) {
            std::cout << "[ModLoader] Loading control.lua for: " << mod->name << std::endl;
            auto result = lua.safe_script_file(controlPath.string(), sol::script_default_on_error);
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua Error Control Stage] " << mod->name << ": " << err.what() << std::endl;
            }
        }
    }
}