# Исправление ошибки `register_object`

## Ошибка
```
[Event Error] ready: ./mods\basic_player\control.lua:25: stack index 1, expected userdata, received sol.sol::d::u<Sprite2D>: value is a userdata but is not the correct unique usertype (bad argument into 'void(std::shared_ptr<Object>)')
```

## Анализ
Ошибка возникает при вызове `Network.register_object(player)` в файле `control.lua`. 

Причина в том, что Lua не может корректно преобразовать `player` (который является `Sprite2D` из Lua) в `Ref<Object>` на стороне C++. 

Несмотря на то, что `Sprite2D` наследует от `Object`, система привязки Lua (`sol2`) не может автоматически выполнить downcast от `Sprite2D` к `Object`, потому что `register_object` ожидает именно `Ref<Object>`, а не `Ref<Sprite2D>`, даже если они семантически совместимы.

Код в `LuaBinder.cpp` пытается преобразовать объект напрямую:
```cpp
net_table["register_object"] = [](Ref<Object> obj) {
    Network::get().register_object(obj);
};
```

Но `sol2` строго проверяет тип, и `Ref<Sprite2D>` не считается автоматически `Ref<Object>`, несмотря на наследование.

## Временное решение
Строка с `Network.register_object(player)` была закомментирована в `control.lua`, чтобы игра запускалась без ошибок.

## Постоянное решение
Необходимо изменить привязку Lua в `LuaBinder.cpp`. Вместо прямого ожидания `Ref<Object>`, нужно:

1. Принимать `sol::object` и вручную выполнять безопасное приведение типов, используя `dynamic_cast` через `static_cast` на уровне указателей.
2. Или создать отдельную перегрузку для `Ref<GameObject2D>` и `Ref<Sprite2D>`, если это допустимо по логике сети.

Пример исправления в `LuaBinder.cpp`:
```cpp
net_table["register_object"] = [](sol::object obj) {
    if (auto ptr = cast_from_lua<GameObject2D>(obj)) {
        auto obj_ref = Ref<Object>(ptr._get(), [](Object*){});
        Network::get().register_object(obj_ref);
    } else {
        throw std::invalid_argument("register_object: expected an Object or GameObject2D derived type");
    }
};
```

Где `cast_from_lua` — это вспомогательная функция, которая уже существует в коде для других типов.

После этого изменения, `register_object` сможет принимать `Sprite2D` и другие производные объекты.