#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "interaction/camera/camera.hpp"
#include "interaction/character/character.hpp"
#include "interaction/input_snapshot/input_snapshot.hpp"
#include "interaction/mouse/mouse.hpp"
#include "interaction/physics/physics.hpp"
#include "interaction/update.hpp"
#include "interaction/window/window.hpp"

#include "game_loop/game_loop.hpp"

#include "graphics/graphics.hpp"
#include "graphics/model_loading/model_loading.hpp"
#include "graphics/physics_debug_renderer/physics_debug_renderer.hpp"
#include "graphics/shader_pipeline/shader_pipeline.hpp"
#include "graphics/window/window.hpp"

#include <Jolt/Physics/Character/Character.h> // temporary

unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

const float movement_acceleration = 15.0f;

InputSnapshot input_snapshot;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct InitializationData {
  ShaderPipeline shader_pipeline;
  Model model;
  GLFWwindow *window;
};

InitializationData initialize(Physics *physics, Mouse *mouse, Camera *camera) {
  GLFWwindow *window =
      initialize_glfw_glad_and_return_window(&SCR_WIDTH, &SCR_HEIGHT, "mwe physics character", false, &input_snapshot);

  glEnable(GL_DEPTH_TEST); // configure global opengl state
  ShaderPipeline shader_pipeline("../graphics/shaders/CWL_v_transformation_with_texture_position_passthrough.vert",
                                 "../graphics/shaders/textured.frag"); // build and compile shaders

  Model model("../assets/maps/ground_test.obj", shader_pipeline.shader_program_id);
  physics->load_model_into_physics_world(&model);
  InitializationData initialization_data = {shader_pipeline, model, window};

  return initialization_data;
}

std::function<int()> termination_closure(GLFWwindow *window) {
  return [window]() { return glfwWindowShouldClose(window); };
}

int main() {

  Character character;
  Camera camera;
  Mouse mouse;
  Physics physics;

  auto [shader_pipeline, model, window] = initialize(&physics, &mouse, &camera);
  PhysicsDebugRenderer physics_debug_renderer;

  std::function<void()> render = create_render_closure(&shader_pipeline, &model, &physics, &camera, window,
                                                       &physics_debug_renderer, SCR_WIDTH, SCR_HEIGHT);
  std::function<void(double)> update =
      create_update_closure(&physics, &camera, &input_snapshot, &mouse, movement_acceleration);
  std::function<int()> termination = termination_closure(window);

  GameLoop game_loop;

  game_loop.start(60, update, render, termination);

  glfwTerminate(); // glfw: terminate, clearing all previously allocated GLFW resources.
  return 0;
}
