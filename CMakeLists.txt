cmake_minimum_required(VERSION 3.25)
project(GraphMaker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(GraphMaker
        src/main.cpp
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_impl_sdl2.cpp
        vendor/imgui/imgui_impl_sdlrenderer.cpp
        )

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/vendor/sdl/lib/libSDL2.dll.a)

include_directories(${PROJECT_NAME}
        vendor/imgui
        vendor/sdl/include/SDL2)

