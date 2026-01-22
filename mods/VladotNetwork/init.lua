-- =======================================================================
-- Network Framework (Library for other mods)
-- =======================================================================

-- Создаем глобальный API объект, который другие моды будут использовать
NF = {} 
NF.localPlayer = nil
NF.objectRegistry = {} -- Локальный реестр объектов

-- =======================================================================
-- ВНУТРЕННИЕ ОБРАБОТЧИКИ RPC (Private)
-- =======================================================================

-- Глобальная функция, которую вызывает C++ NetworkManager через RPC (спавн объекта)
function handle_spawn(data_str, obj_id, sender_id)
    local data = json.parse(data_str)
    
    -- Проверяем, существует ли уже объект (чтобы избежать дублирования на сервере)
    if Network.find_object_by_id(obj_id) ~= nil then return end

    -- local new_obj = data.factory_func(data) <- Идеально, но сложно прокинуть функции через RPC
    -- Вместо этого другие моды должны предоставить свою функцию спавна через NF.registerSpawner()

    if NF.spawnerFunc then
        local new_obj = NF.spawnerFunc(data, obj_id)
        if new_obj then
            new_obj.network_id = obj_id
            Network.register_object(new_obj)
            NF.objectRegistry[obj_id] = new_obj
            Log.info("[NF] Spawned: " .. new_obj:get_class_name() .. " ID: " .. obj_id)
        end
    else
        Log.err("[NF] No spawner function registered! Cannot spawn object ID: " .. obj_id)
    end
end

-- Глобальная функция, вызываемая по RPC для выполнения команд на объектах
function handle_rpc_call(data_str, obj_id, sender_id)
    local data = json.parse(data_str)
    local target = Network.find_object_by_id(obj_id)

    if target and target[data.func_name] then
        -- Вызываем метод объекта в Lua
        target[data.func_name](target, data.args)
    -- else
        -- Log.warn("[NF] RPC call for unknown object: " .. obj_id .. " Func: " .. data.func_name)
    end
end


-- =======================================================================
-- ПУБЛИЧНЫЙ API ДЛЯ ДРУГИХ МОДОВ (Public)
-- =======================================================================

--- Устанавливает функцию обратного вызова для создания объектов по сети.
--- @param func function(data, obj_id):GameObject Функция должна вернуть созданный объект
function NF.registerSpawner(func)
    NF.spawnerFunc = func
end

--- Отправляет команду RPC конкретному объекту (локально вызовет handle_rpc_call)
--- @param object GameObject Целевой объект
--- @param func_name string Имя функции объекта для вызова
--- @param args table Аргументы функции (будут сериализованы в JSON)
--- @param target_peer_id integer|nil ID пира-получателя (-1 для всех)
function NF.sendObjectRPC(object, func_name, args, target_peer_id)
    if object.network_id == -1 then
        Log.err("[NF] Cannot send RPC for object without network_id!")
        return
    end

    local payload = { func_name = func_name, args = args }
    -- Отправляем RPC, который вызовет на клиентах функцию handle_rpc_call
    Network.send_rpc("handle_rpc_call", payload, object.network_id, target_peer_id or -1)
end

--- Спавнит объект по сети (вызывать только на сервере)
--- @param data table Данные, необходимые для спавна (будут переданы в spawnerFunc)
function NF.spawnNetworkObject(data)
    if Network.get_my_id() ~= 0 then
        Log.err("[NF] spawnNetworkObject can only be called by the server!")
        return
    end

    local new_id = Network.generate_next_object_id()
    
    -- Вызываем локальный хендлер напрямую на сервере
    handle_spawn(json.encode(data), new_id, 0)

    -- Отправляем RPC всем клиентам
    Network.send_rpc("handle_spawn", data, new_id)
end

-- Добавляем API в глобальную область видимости для удобства
_G.NF = NF
