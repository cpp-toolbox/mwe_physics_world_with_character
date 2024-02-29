#ifndef INPUT_SNAPSHOT_HPP
#define INPUT_SNAPSHOT_HPP

#include <glm/glm.hpp>

#include "../camera/camera.hpp"

class InputSnapshot {
public:
    bool left_pressed = false;
    bool right_pressed = false;
    bool forward_pressed = false;
    bool backward_pressed = false;
    bool jump_pressed = false;
    glm::vec3 input_snapshot_to_input_direction(Camera camera);
};

#endif //INPUT_SNAPSHOT_HPP