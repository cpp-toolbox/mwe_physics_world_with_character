#include "input_snapshot.hpp"

/**
 * given an input snapshot, return a normalized vector which represents how the pressed
 * keys should change our movement relative to the current look direction
 *
 * ex) pressing forward should eventually move the player in the direction they're looking
 *
 */
glm::vec3 InputSnapshot::input_snapshot_to_input_direction(Camera camera) {

    // TODO calling everything a compmoent is confusing af, fix this
    float forward_component = (float) this->forward_pressed - (float) this->backward_pressed;
    float strafe_component = (float) this->right_pressed - (float) this->left_pressed;

    // std::cout << glm::to_string(player.camera.look_direction) << std::endl;

    glm::vec3 xz_player_look_direction = glm::vec3(camera.look_direction.x, 0.0f,  camera.look_direction.z);

    if (glm::length(xz_player_look_direction) != 0) {
        xz_player_look_direction = glm::normalize(xz_player_look_direction);
    }

    // TODO once this is in the player class we will be using player look direction, then doing stuff the camera with that information
    glm::vec3 forward_input_component = forward_component * xz_player_look_direction;
    glm::vec3 strafe_direction = glm::cross(xz_player_look_direction, camera.up_direction);
    glm::vec3 strafe_input_component = strafe_component * strafe_direction;
    glm::vec3 input_vector = forward_input_component + strafe_input_component;

    if (glm::length(input_vector) == 0) {
        return input_vector;
    } else {
        return glm::normalize(input_vector);
    }
}
