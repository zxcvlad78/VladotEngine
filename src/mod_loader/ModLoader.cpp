#include "ModLoader.hpp"
#include <set>
#include <algorithm>

/**
 * Сканирование папки модов.
 * Монтирует папки в VFS и подготавливает список для сортировки.
 */
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
                    m.path = entry.path(); // Сохраняем физический путь к папке мода
                    
                    if (data.contains("dependencies")) {
                        m.dependencies = data["dependencies"].get<std::vector<std::string>>();
                    }
                    
                    availableMods[m.name] = m;
                    
                    // Монтируем папку мода в корень VFS для доступа к ассетам (текстурам и т.д.)
                    vfs->mount(entry.path().string(), VirtualFS::FOLDER);
                    
                    std::cout << "[ModLoader] Found Mod: " << m.name << " v" << m.version << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[ModLoader] Error parsing info.json in " << entry.path() << ": " << e.what() << std::endl;
                }
            }
        }
    }

    // Выстраиваем порядок загрузки на основе зависимостей
    resolve_dependencies();
}

/**
 * Топологическая сортировка (алгоритм поиска в глубину)
 */
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

/**
 * Этап загрузки прототипов (data.lua)
 */
void ModLoader::load_data_stage(sol::state& lua, Engine::IRegistry* registry, VirtualFS* vfs) {
    auto data_table = lua.create_table();
    data_table["extend"] = [registry](sol::table config) { 
        registry->register_prototype(config); 
    };
    lua["data"] = data_table;

    for (auto* mod : sortedLoadOrder) {
        // Загружаем data.lua напрямую через путь мода
        std::filesystem::path dataScriptPath = mod->path / "data.lua";
        
        if (std::filesystem::exists(dataScriptPath)) {
            std::cout << "[ModLoader] Loading data.lua for: " << mod->name << std::endl;
            sol::protected_function_result result = lua.do_file(dataScriptPath.string());
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua Error Data Stage] " << mod->name << ": " << err.what() << std::endl;
            }
        }
    }
}

/**
 * Этап загрузки игровой логики (control.lua)
 */
void ModLoader::load_control_stage(sol::state& lua, Engine::IGameplayAPI* gameplay, Engine::EventSystem* events, VirtualFS* vfs) {
    auto events_table = lua.create_table();
    
    // Поддержка event_system:on("event", function)
    events_table["on"] = [events](sol::table self, const std::string& eventName, sol::function callback) {
        events->subscribe(eventName, std::move(callback));
    };
    
    lua["event_system"] = events_table;

    for (auto* mod : sortedLoadOrder) {
        // Загружаем control.lua напрямую через путь мода
        std::filesystem::path controlScriptPath = mod->path / "control.lua";
        
        if (std::filesystem::exists(controlScriptPath)) {
            std::cout << "[ModLoader] Loading control.lua for: " << mod->name << std::endl;
            
            // Запускаем скрипт
            sol::protected_function_result result = lua.do_file(controlScriptPath.string());
            
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "[Lua Error Control Stage] " << mod->name << ": " << err.what() << std::endl;
            }
        } else {
            // Если файла нет, это нормально (мод может быть только на ассеты или данные)
            std::cout << "[ModLoader] Skipping control.lua for " << mod->name << " (not found)" << std::endl;
        }
    }
}
