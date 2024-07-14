#ifndef MWE_MODEL_LOADING_GRAPHICS_HPP
#define MWE_MODEL_LOADING_GRAPHICS_HPP

#ifndef JPH_DEBUG_RENDERER
#define JPH_DEBUG_RENDERER
#endif

#include "shader_pipeline/shader_pipeline.hpp"
#include "textured_model_loading/textured_model_loading.hpp"
#include "../interaction/physics/physics.hpp"
#include "../interaction/camera/camera.hpp"
#include "physics_debug_renderer/physics_debug_renderer.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

// eventually physics needs to be removed from it too big of an argument
void render(ShaderPipeline *shader_pipeline, Physics *physics, PhysicsDebugRenderer *physics_debug_renderer, Model *map,
            JPH::RVec3 character_position, Camera *camera, int screen_width, int screen_height);

std::function<void()> create_render_closure(ShaderPipeline *shader_pipeline, Model *map, Physics *physics,
                                            Camera *camera, GLFWwindow *window,
                                            PhysicsDebugRenderer *physics_debug_renderer, unsigned int screen_width_px,
                                            unsigned int screen_height_px);

#endif // MWE_MODEL_LOADING_GRAPHICS_HPP
