cmake_minimum_required(VERSION 3.10)
project(Raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(raytracer
    main.cpp
    scene.cpp
)

target_link_libraries(raytracer
    sfml-graphics
    sfml-window
    sfml-system
) 