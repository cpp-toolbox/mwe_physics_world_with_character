#ifndef MWE_MODEL_LOADING_GRAPHICS_HPP
#define MWE_MODEL_LOADING_GRAPHICS_HPP

#ifndef JPH_DEBUG_RENDERER
#define JPH_DEBUG_RENDERER
#endif

#include "shader_pipeline/shader_pipeline.hpp"
#include "../graphics/model_loading/model_loading.hpp"
#include "../interaction/physics/physics.hpp"
#include "../interaction/camera/camera.hpp"
#include "physics_debug_renderer/physics_debug_renderer.hpp"

// eventually physics needs to be removed from it too big of an argument
void render(ShaderPipeline shader_pipeline, Physics *physics, PhysicsDebugRenderer *physics_debug_renderer, Model model, JPH::RVec3 character_position, Camera camera, int screen_width, int screen_height);

#endif //MWE_MODEL_LOADING_GRAPHICS_HPP