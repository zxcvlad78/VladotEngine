cmake_minimum_required(VERSION 3.25)

if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

set(X_VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "" FORCE)

project(VladotEngine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/MP /utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()


find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(Lua REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)
find_package(miniz CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/networking/network/Network.cpp
    src/lua_binder/LuaBinder.cpp
    src/mod_loader/ModLoader.cpp
    src/object/scene_tree/SceneTree.cpp
    src/object/resource/texture_resource/TextureResource.cpp
    src/object/resource/shader_resource/ShaderResource.cpp

    src/object/game_object/game_object_2d/GameObject2D.cpp


    src/object/game_object/game_object_2d/sprite_2d/Sprite2D.cpp
    src/object/resource/material/Material.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad::glad
    Freetype::Freetype
    ${LUA_LIBRARIES}
    OpenGL::GL
    ws2_32.lib
    winmm.lib
    glm::glm
    nlohmann_json::nlohmann_json
    sol2::sol2
    unofficial::enet::enet
    miniz::miniz
    imgui::imgui
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${LUA_INCLUDE_DIR}
    ${SOL2_INCLUDE_DIRS}
    ${TERMCOLOR_INCLUDE_DIRS}
    ${Stb_INCLUDE_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/mods" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/mods"
    COMMENT "Resources copied successfully to build directory."
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <string>
    <vector>
    <memory>
    <filesystem>
    <iostream>
    <algorithm>
    <functional>
    <unordered_map>
    <type_traits>
)
