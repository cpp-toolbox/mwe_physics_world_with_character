#include "graphics.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyManager.h>

void bind_CWL_matrix_uniforms(GLuint shader_program_id, int screen_width, int screen_height,
                              glm::vec3 character_position, Camera *camera);

void render(ShaderPipeline *shader_pipeline, Physics *physics, PhysicsDebugRenderer *physics_debug_renderer, Model *map,
            JPH::RVec3 character_position, Camera *camera, int screen_width, int screen_height) {
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::vec3 glm_character_position = glm::vec3(character_position.GetX(), character_position.GetY(),
                                               character_position.GetZ()); // TEMP use helpers after

  bind_CWL_matrix_uniforms(shader_pipeline->shader_program_id, screen_width, screen_height, glm_character_position,
                           camera);
  bind_CWL_matrix_uniforms(physics_debug_renderer->shader_pipeline.shader_program_id, screen_width, screen_height,
                           glm_character_position, camera);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draw in wireframe

  map->draw();

  JPH::BodyManager::DrawSettings draw_settings;
  draw_settings.mDrawShapeWireframe = true;
  physics->physics_system.DrawBodies(draw_settings, physics_debug_renderer);
}

std::function<void()> create_render_closure(ShaderPipeline *shader_pipeline, Model *map, Physics *physics,
                                            Camera *camera, GLFWwindow *window,
                                            PhysicsDebugRenderer *physics_debug_renderer, unsigned int screen_width_px,
                                            unsigned int screen_height_px) {
  return [shader_pipeline, map, physics, window, camera, physics_debug_renderer, screen_width_px, screen_height_px]() {
    JPH::RVec3 character_position = physics->character->GetPosition();
    render(shader_pipeline, physics, physics_debug_renderer, map, character_position, camera, screen_width_px,
           screen_height_px);
    glfwSwapBuffers(window);
    glfwPollEvents();
  };
}

/**
 *
 * \brief binds matrices into uniforms given the shader program's id
 *
 * Specifically world_to_camera translates the world coordinate system to have
 * origin centered at the characters position, facing in the direction that the
 * character is looking
 *
 * It binds camera_to_clip to a basic perspective transformation
 *
 * It binds local_to_world as the identity, which can be overridden if needed
 *
 * \precondition the shader program uses a CWL transformation with attribute
 * names:
 * - camera_to_clip
 * - world_to_camera
 * - local_to_world
 *
 * \param shader_program_id
 * \param screen_width
 * \param screen_height
 * \param character_position
 */
void bind_CWL_matrix_uniforms(GLuint shader_program_id, int screen_width, int screen_height,
                              glm::vec3 character_position, Camera *camera) {

  // don't forget to enable shader before setting uniforms
  glUseProgram(shader_program_id);

  // view/projection transformations
  glm::mat4 camera_to_clip =
      glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
  GLint camera_to_clip_uniform_location = glGetUniformLocation(shader_program_id, "camera_to_clip");
  glUniformMatrix4fv(camera_to_clip_uniform_location, 1, GL_FALSE, glm::value_ptr(camera_to_clip));

  glm::mat4 world_to_camera =
      glm::lookAt(character_position, character_position + camera->look_direction, camera->up_direction);
  GLint world_to_camera_uniform_location = glGetUniformLocation(shader_program_id, "world_to_camera");
  glUniformMatrix4fv(world_to_camera_uniform_location, 1, GL_FALSE, glm::value_ptr(world_to_camera));

  // render the loaded model
  glm::mat4 local_to_world = glm::mat4(1.0f);
  //    local_to_world = glm::translate(local_to_world, glm::vec3(0.0f, 0.0f,
  //    0.0f)); // translate it down so it's at the center of the scene
  //    local_to_world = glm::scale(local_to_world,
  //    glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so
  //    scale it down

  GLint local_to_world_uniform_location = glGetUniformLocation(shader_program_id, "local_to_world");
  glUniformMatrix4fv(local_to_world_uniform_location, 1, GL_FALSE, glm::value_ptr(local_to_world));
}
