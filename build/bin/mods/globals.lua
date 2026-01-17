--- Глобальные типы данных проекта (C++ классы, обернутые в Lua)

--- @class Resource Базовый класс для всех ресурсов
--- @field id number Уникальный ID ресурса (OpenGL/OpenAL ID)
--- @field type string Тип ресурса ("texture", "audio")
--- @field path string Виртуальный путь к файлу

--- @class ResourceAPI Менеджер загрузки ресурсов
--- @field load fun(path: string, type: string): Resource Загружает ресурс по указанному типу
--- @field load_auto fun(path: string): Resource Автоматически определяет тип и загружает ресурс

--- @class GameplayAPI API для взаимодействия с игровым миром
--- @field spawn_mob fun(name: string, x: number, y: number) Создает нового моба в мире

--- @class EventSystem API для подписки на события
--- @field subscribe fun(eventName: string, callback: function) Подписывает функцию на событие

--- Глобальные переменные, доступные в control.lua
--- @type GameplayAPI
script = script

--- @type ResourceAPI
resources = resources

--- @type EventSystem
events = events

--- @class DataTable API для регистрации прототипов данных
--- @field extend fun(config: table) Регистрирует новый прототип (моб, предмет, рецепт)

--- @type DataTable
data = data
