cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mwe_physics_world_with_character)

add_executable(mwe_physics_world_with_character
        src/main.cpp

        src/interaction/physics/physics.cpp
        src/interaction/camera/camera.cpp
        src/interaction/mouse/mouse.cpp
        src/interaction/update.cpp

        src/game_loop/game_loop.cpp

        src/math/conversions.cpp

        src/utils/stopwatch/stopwatch.cpp

        src/graphics/physics_debug_renderer/physics_debug_renderer.cpp
        src/graphics/shader_pipeline/shader_pipeline.cpp
        src/graphics/textured_model_loading/textured_model_loading.cpp
        src/graphics/window/window.cpp
        src/graphics/graphics.cpp
)

# Jolt Physics: Physics Engine

include_directories(external_libraries/JoltPhysics)
add_subdirectory(external_libraries/JoltPhysics/Build)

find_package(glfw3)
find_package(glad)
find_package(assimp)
find_package(stb)

target_link_libraries(${PROJECT_NAME} glfw glad::glad assimp::assimp stb::stb Jolt)