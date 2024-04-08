#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <optional>

#include "interaction/camera/camera.hpp"
#include "interaction/character/character.hpp"
#include "interaction/input_snapshot/input_snapshot.hpp"
#include "interaction/mouse/mouse.hpp"
#include "interaction/physics/physics.hpp"

#include "math/conversions.hpp"

#include "graphics/graphics.hpp"
#include "graphics/model_loading/model_loading.hpp"
#include "graphics/physics_debug_renderer/physics_debug_renderer.hpp"
#include "graphics/shader_pipeline/shader_pipeline.hpp"
#include "graphics/window/window.hpp"

#include <Jolt/Physics/Character/Character.h> // temporary

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const float movement_acceleration = 15.0f;

InputSnapshot input_snapshot;
Character character;
Camera camera;
Mouse mouse;
Physics physics;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
void on_window_size_change(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void on_mouse_move(GLFWwindow *window, double mouse_position_x, double mouse_position_y) {
  auto [change_in_yaw_angle, change_in_pitch_angle] = mouse.get_yaw_pitch_deltas(mouse_position_x, mouse_position_y);
  camera.update_look_direction(change_in_yaw_angle, change_in_pitch_angle);
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
static void process_input(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // TODO figure out how to do mappings of key to function to simplify
  //
  if (key == GLFW_KEY_Q) {
    if (action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  } else if (key == GLFW_KEY_F) {
    // TOGGLE FULLSCREEN
    // https://stackoverflow.com/a/47462358/6660685 <- implement this
  } else if (key == GLFW_KEY_A) {
    if (action == GLFW_PRESS) {
      input_snapshot.left_pressed = true;
    } else if (action == GLFW_RELEASE) {
      input_snapshot.left_pressed = false;
    }
  } else if (key == GLFW_KEY_D) {
    if (action == GLFW_PRESS) {
      input_snapshot.right_pressed = true;
    } else if (action == GLFW_RELEASE) {
      input_snapshot.right_pressed = false;
    }
  } else if (key == GLFW_KEY_W) {
    if (action == GLFW_PRESS) {
      input_snapshot.forward_pressed = true;
    } else if (action == GLFW_RELEASE) {
      input_snapshot.forward_pressed = false;
    }
  } else if (key == GLFW_KEY_S) {
    if (action == GLFW_PRESS) {
      input_snapshot.backward_pressed = true;
    } else if (action == GLFW_RELEASE) {
      input_snapshot.backward_pressed = false;
    }
  } else if (key == GLFW_KEY_SPACE) {
    if (action == GLFW_PRESS) {
      input_snapshot.jump_pressed = true;
    } else if (action == GLFW_RELEASE) {
      input_snapshot.jump_pressed = false;
    }
  }
}

struct InitializationData {
  ShaderPipeline shader_pipeline;
  Model model;
  GLFWwindow *window;
};

std::optional<InitializationData> initialize() {

  auto optional_window = initialize_glfw_and_return_window(SCR_WIDTH, SCR_HEIGHT);

  if (!optional_window.has_value()) {
    return std::nullopt;
  }

  GLFWwindow *window = optional_window.value();

  glfwSetFramebufferSizeCallback(window, on_window_size_change);
  glfwSetCursorPosCallback(window, on_mouse_move);
  glfwSetKeyCallback(window, process_input);
  mouse.configure_with_glfw(window);

  glEnable(GL_DEPTH_TEST); // configure global opengl state

  ShaderPipeline shader_pipeline("../graphics/shaders/CWL_v_transformation_with_texture_position_passthrough.vert",
                                 "../graphics/shaders/textured.frag"); // build and compile shaders

  Model model("../assets/maps/ground_test.obj", shader_pipeline.shader_program_id);

  physics.load_model_into_physics_world(&model);

  InitializationData initialization_data = {shader_pipeline, model, window};

  return initialization_data;
}

void update(double time_since_last_update) {

  // in jolt y is z
  glm::vec3 updated_velocity = convert_vec3_from_jolt_to_glm(physics.character->GetLinearVelocity());

  glm::vec3 input_vec = input_snapshot.input_snapshot_to_input_direction(camera);

  updated_velocity += input_vec * movement_acceleration * (float)time_since_last_update;

  glm::vec3 current_xz_velocity = glm::vec3(updated_velocity.x, 0.0f, updated_velocity.z);

  glm::vec3 y_axis = glm::vec3(0, 1, 0);

  float friction = 0.983f;

  updated_velocity *= friction; // friction

  // apply gravity
  updated_velocity +=
      convert_vec3_from_jolt_to_glm(physics.physics_system.GetGravity()) * (float)time_since_last_update;

  // jump if needed.
  if (input_snapshot.jump_pressed &&
      physics.character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
    updated_velocity += (float)10 * convert_vec3_from_jolt_to_glm(physics.character->GetUp());
  }

  physics.character->SetLinearVelocity(convert_vec3_from_glm_to_jolt(updated_velocity));

  physics.update(time_since_last_update);
}

int main() {

  std::optional<InitializationData> id = initialize();

  if (!id.has_value()) {
    return -1;
  }
  auto [shader_pipeline, model, window] = id.value();

  PhysicsDebugRenderer physics_debug_renderer;

  //  todo extract game loop into a function which takes in function pointers and runs them in specific places within
  //  the loop

  double time_elapsed_since_start_of_program = 0;
  // N iterations per second
  double update_rate_hz = 60.0;
  // 1/N seconds per iteration
  double time_between_state_update = 1.0 / update_rate_hz;

  double time_elapsed_since_last_state_update = 0;

  bool first_iteration = true;
  bool first_update = true;

  double time_at_start_of_iteration_last_iteration = -1.0;
  double duration_of_last_iteration = -1.0;

  while (!glfwWindowShouldClose(window)) {

    double time_at_start_of_iteration = glfwGetTime(); // (T)

    if (first_iteration) {
      // The last few lines of this iteration are next loops last iteration.
      first_iteration = false;
      time_at_start_of_iteration_last_iteration = time_at_start_of_iteration; // (C)
      time_elapsed_since_last_state_update = time_at_start_of_iteration;      // (F): Pretend an update has occurred at
                                                                              // time 0 for bootstrapping purposes
      continue;
    }

    // Note that this measures how long it takes for the code to start at (T)
    // and arrive back at (T) (G): Due to (C) tesli == 0 on the second iteration
    duration_of_last_iteration = time_at_start_of_iteration - time_at_start_of_iteration_last_iteration;

    // None of the updates that could have happened during the last iteration
    // have been applied This is because last iteration, we retroactively
    // applied last iterations updates
    time_elapsed_since_last_state_update += duration_of_last_iteration;

    // since the value of teslsu is only updated by (E), this would always be
    // false, but (F) bootstraps the process
    bool enough_time_for_updates = time_elapsed_since_last_state_update >= time_between_state_update;

    // Due to the (G), an update could only happen starting from the 3rd
    // iteration
    if (enough_time_for_updates) {

      // retroactively apply updates that should have occurred during previous
      // iterations
      double time_remaining_to_fit_updates = time_elapsed_since_last_state_update;
      bool enough_time_to_fit_update = true;

      while (enough_time_to_fit_update) {
        // TEMP
        update(time_between_state_update);

        time_remaining_to_fit_updates -= time_between_state_update;
        enough_time_to_fit_update = time_remaining_to_fit_updates >= time_between_state_update;
      }
      time_elapsed_since_last_state_update = time_remaining_to_fit_updates;
    }

    JPH::RVec3 character_position = physics.character->GetPosition();
    render(shader_pipeline, &physics, &physics_debug_renderer, model, character_position, camera, SCR_WIDTH, SCR_HEIGHT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    // With respect to the start of the next iteration, the code down here is
    // previous iteration.
    time_at_start_of_iteration_last_iteration = time_at_start_of_iteration;
  }

  glfwTerminate(); // glfw: terminate, clearing all previously allocated GLFW
                   // resources.
  return 0;
}
