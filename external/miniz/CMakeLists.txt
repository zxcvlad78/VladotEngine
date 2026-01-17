project(miniz C)

add_library(miniz STATIC miniz.c)

target_include_directories(miniz PUBLIC .)