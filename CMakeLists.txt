cmake_minimum_required(VERSION 3.10)

project(mwe_physics_world_with_character)

add_executable(mwe_physics_world_with_character
        main.cpp

        interaction/input/input_snapshot.cpp
        interaction/physics/physics.cpp
        interaction/camera/camera.cpp
        interaction/mouse/mouse.cpp

        helpers/conversions.cpp

        graphics/physics_debug_renderer/physics_debug_renderer.cpp
        graphics/shader_pipeline/shader_pipeline.cpp
        graphics/model_loading/model_loading.cpp
        graphics/window/window.cpp
        graphics/graphics.cpp
)

# GLAD: opengl function loader

include_directories(external_libraries/glad_opengl_3.3_core/include)
add_subdirectory(external_libraries/glad_opengl_3.3_core)

# GLM: opengl mathematics

include_directories(external_libraries/glm)
add_subdirectory(external_libraries/glm)

# GLFW: opengl framework

# disable unnessary steps in build
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external_libraries/glfw)

# ASSIMP: Asset Importer

# only use obj to speed up compilation
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

include_directories(external_libraries/assimp/include)
add_subdirectory(external_libraries/assimp)

# STB_H: single file libraries
include_directories(external_libraries/stb)

# Jolt Physics: Physics Engine

include_directories(external_libraries/JoltPhysics)
add_subdirectory(external_libraries/JoltPhysics/Build)

target_link_libraries(mwe_physics_world_with_character glfw glad assimp Jolt)