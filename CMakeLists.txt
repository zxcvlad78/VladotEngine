cmake_minimum_required(VERSION 4.0.2 FATAL_ERROR)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "Z:/dev/cpp/Vladot/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

set(PROJECT_NAME VladotEngine)
project(${PROJECT_NAME})

add_definitions(-DLUA_VERSION_NUM=504) 

if(MINGW OR CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options("-Wa,-mbig-obj")
endif()

# GLFW Settings
set(GLFW_BUILDS_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILDS_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# SubDirs
add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/freetype)
add_subdirectory(external/lua)
add_subdirectory(external/miniz)
add_subdirectory(external/enet)

## Exec
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/networking/network/Network.cpp
    #src/virtual_fs/VirtualFS.cpp
    src/lua_binder/LuaBinder.cpp
    src/mod_loader/ModLoader.cpp
    src/object/scene_tree/SceneTree.cpp
    src/object/resource/texture_resource/TextureResource.cpp
    src/object/resource/shader_resource/ShaderResource.cpp
    src/object/game_object/game_object_2d/sprite_2d/Sprite2D.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20) 

target_link_libraries(${PROJECT_NAME} 
    glfw 
    glad 
    freetype
    lua
    opengl32
    miniz
    enet
    ws2_32
    winmm
)

# INCLUDE DIRS
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/freetype/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/lua
    ${CMAKE_SOURCE_DIR}/external/lua/src
    ${CMAKE_SOURCE_DIR}/external/sol2/include
    ${CMAKE_SOURCE_DIR}/external/json/include
    ${CMAKE_SOURCE_DIR}/external/miniz
    ${CMAKE_SOURCE_DIR}/external/termcolor/include
    ${CMAKE_SOURCE_DIR}/external/enet/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/mods $<TARGET_FILE_DIR:${PROJECT_NAME}>/mods)

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <sol/sol.hpp>
    <glm/glm.hpp>
    <glm/gtc/matrix_transform.hpp>
    <glm/gtc/type_ptr.hpp>
    <string>
    <vector>
    <memory>
    <filesystem>
    <iostream>
    <algorithm>
    <nlohmann/json.hpp>
    <enet/enet.h>
)
