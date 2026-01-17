#include "ModLoader.hpp"

void ModLoader::scan_mods(const std::filesystem::path& folder) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(folder)) {
        std::filesystem::path infoPath = entry.path() / "info.json";
        if (std::filesystem::exists(infoPath)) {
            std::ifstream f(infoPath);
            try {
                json data = json::parse(f);
                ModEntry m;
                m.name = data["name"];
                m.version = data["version"];
                m.path = entry.path();
                if (data.contains("dependencies")) {
                    m.dependencies = data["dependencies"].get<std::vector<std::string>>();
                }
                availableMods[m.name] = m;
                std::cout << "[ModLoader] Found Mod: " << m.name << " v" << m.version << std::endl;
            } catch (...) {
                std::cerr << "[ModLoader] Error Parsing info.json в " << entry.path() << std::endl;
            }
        }
    }
    // Здесь должна быть топологическая сортировка, пока просто берем все найденные моды
    for(auto& [name, mod] : availableMods) {
        sortedLoadOrder.push_back(&mod);
    }
}

void ModLoader::load_data_stage(sol::state& lua, Engine::IRegistry* registry, VirtualFS* vfs) {
    auto data_table = lua.create_table();
    data_table["extend"] = [&](sol::table config) { registry->register_prototype(config); };
    lua["data"] = data_table;

    for (auto* mod : sortedLoadOrder) {
        std::string virtualPath = mod->name + "/data.lua"; 
        
        std::string scriptContent = vfs->read_file_string(virtualPath);
        
        if (!scriptContent.empty()) {
            std::cout << "[ModLoader] Loading data.lua for " << mod->name << std::endl;
            lua.script(scriptContent); 
        }
    }
}

void ModLoader::load_control_stage(sol::state& lua, Engine::IGameplayAPI* gameplay, Engine::EventSystem* events, VirtualFS* vfs) {
    auto script_table = lua.create_table();
    lua["script"] = script_table;

    auto events_table = lua.create_table();
    events_table["subscribe"] = [&](const std::string& eventName, sol::function callback) {
        events->subscribe(eventName, std::move(callback));
    };
    lua["events"] = events_table;

    for (auto* mod : sortedLoadOrder) {
        std::string virtualPath = mod->name + "/control.lua"; 
        
        std::string scriptContent = vfs->read_file_string(virtualPath);
        
        if (!scriptContent.empty()) {
            std::cout << "[ModLoader] Loading control.lua for " << mod->name << std::endl;
            lua.script(scriptContent);
        }
    }
}
