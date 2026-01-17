cmake_minimum_required(VERSION 4.0.2 FATAL_ERROR)

set(PROJECT_NAME VladotEngine)

project(${PROJECT_NAME})


add_executable(${PROJECT_NAME} 
    src/main.cpp
    )

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(GLFW_BUILDS_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILDS_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

#SubDirs
add_subdirectory(external/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(external/glad)
target_link_libraries(${PROJECT_NAME} glad)

add_subdirectory(external/freetype)
target_link_libraries(${PROJECT_NAME} freetype)
##

#INCLUDE DIRS
target_include_directories(${PROJECT_NAME} PRIVATE ${glm_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/freetype/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/mods $<TARGET_FILE_DIR:${PROJECT_NAME}>/mods)