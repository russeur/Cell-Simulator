cmake_minimum_required(VERSION 4.1.0)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
project(Galaga VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
add_compile_options(-Wall -g -O3)


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
src/AdhesionConnection.cpp
src/DNA.cpp
src/MyGui.cpp

dependencies/imgui/imgui.cpp
dependencies/imgui/imgui_draw.cpp
dependencies/imgui/imgui_tables.cpp
dependencies/imgui/imgui_widgets.cpp
dependencies/imgui/backends/imgui_impl_glfw.cpp
dependencies/imgui/backends/imgui_impl_opengl3.cpp
dependencies/imgui/implot.cpp
dependencies/imgui/implot_items.cpp
)

target_include_directories(main PRIVATE dependencies headers dependencies/imgui dependencies/imgui/backends)


target_link_directories(main PRIVATE lib)

target_link_libraries(main glfw OpenGL::GL)




