#include "../math/conversions.hpp"
#include "update.hpp"

void update(double time_since_last_update, Physics *physics, Camera *camera, InputSnapshot *input_snapshot,
            Mouse *mouse, float movement_acceleration) {

  auto [change_in_yaw_angle, change_in_pitch_angle] =
      mouse->get_yaw_pitch_deltas(input_snapshot->mouse_position_x, input_snapshot->mouse_position_y);
  camera->update_look_direction(change_in_yaw_angle, change_in_pitch_angle);

  // in jolt y is z
  glm::vec3 updated_velocity = convert_vec3_from_jolt_to_glm(physics->character->GetLinearVelocity());

  glm::vec3 input_vec = input_snapshot->input_snapshot_to_input_direction(*camera);

  updated_velocity += input_vec * movement_acceleration * (float)time_since_last_update;

  glm::vec3 current_xz_velocity = glm::vec3(updated_velocity.x, 0.0f, updated_velocity.z);

  glm::vec3 y_axis = glm::vec3(0, 1, 0);

  float friction = 0.983f;

  updated_velocity *= friction; // friction

  // jump if needed.
  if (physics->character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
    updated_velocity.y = 0; // empty out vertical velocity while on ground
    if (input_snapshot->jump_pressed) {
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

std::function<void(double)> create_update_closure(Physics *physics, Camera *camera, InputSnapshot *input_snapshot,
                                                  Mouse *mouse, float movement_acceleration) {
  return [physics, camera, input_snapshot, mouse, movement_acceleration](float time_since_last_update) {
    update(time_since_last_update, physics, camera, input_snapshot, mouse, movement_acceleration);
  };
}
