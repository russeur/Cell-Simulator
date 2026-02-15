cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
project(Galaga VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
add_compile_options(-Wall)


add_executable(main 
src/config.h 
src/main.cpp
dependencies/glad/glad.c
src/shaderClass.cpp
src/VBOClass.cpp
src/EBOClass.cpp
src/VAOClass.cpp
dependencies/stb/stb.cpp
src/TextureUtil.cpp
src/Window.cpp
src/Position2D.cpp
src/Environment.cpp
src/Cell.cpp
src/Cell_Render.cpp
src/CellManager.cpp
)

target_include_directories(main PRIVATE dependencies headers)


target_link_directories(main PRIVATE lib)

target_link_libraries(main glfw3 OpenGL::GL)




