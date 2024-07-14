#include "../math/conversions.hpp"
#include "update.hpp"

void update(double time_since_last_update, Physics *physics, Camera *camera, LiveInputState *live_input_state,
            Mouse *mouse, float movement_acceleration) {

  auto [change_in_yaw_angle, change_in_pitch_angle] =
      mouse->get_yaw_pitch_deltas(live_input_state->mouse_position_x, live_input_state->mouse_position_y);
  camera->update_look_direction(change_in_yaw_angle, change_in_pitch_angle);

  // in jolt y is z
  glm::vec3 updated_velocity = convert_vec3_from_jolt_to_glm(physics->character->GetLinearVelocity());

  glm::vec3 input_vec = camera->input_snapshot_to_input_direction(live_input_state->forward_pressed, live_input_state->backward_pressed, live_input_state->right_pressed, live_input_state->left_pressed);

  updated_velocity += input_vec * movement_acceleration * (float)time_since_last_update;

  glm::vec3 current_xz_velocity = glm::vec3(updated_velocity.x, 0.0f, updated_velocity.z);

  glm::vec3 y_axis = glm::vec3(0, 1, 0);

  float friction = 0.983f;

  updated_velocity *= friction; // friction

  // jump if needed.
  if (physics->character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
    updated_velocity.y = 0; // empty out vertical velocity while on ground
    if (live_input_state->jump_pressed) {
      updated_velocity +=
          (float)1200 * convert_vec3_from_jolt_to_glm(physics->character->GetUp()) * (float)time_since_last_update;
    }
  }

  glm::vec3 gravity = convert_vec3_from_jolt_to_glm(physics->physics_system.GetGravity());
  // apply gravity
  updated_velocity += gravity * (float)time_since_last_update;

  physics->character->SetLinearVelocity(convert_vec3_from_glm_to_jolt(updated_velocity));
  physics->update(time_since_last_update);
}

std::function<void(double)> create_update_closure(Physics *physics, Camera *camera, LiveInputState *live_input_state,
                                                  Mouse *mouse, float movement_acceleration) {
  return [physics, camera, live_input_state, mouse, movement_acceleration](float time_since_last_update) {
    update(time_since_last_update, physics, camera, live_input_state, mouse, movement_acceleration);
  };
}
