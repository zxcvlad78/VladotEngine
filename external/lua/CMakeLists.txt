project(lua C)

# Указываем CMake искать исходники внутри папки src/
file(GLOB LUA_SOURCES "src/*.c")

# Исключаем ненужные исполняемые файлы из сборки
list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c")

add_library(lua STATIC ${LUA_SOURCES})

# Добавляем корневую папку и src в пути инклудов
target_include_directories(lua PUBLIC . src)
