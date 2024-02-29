#ifndef CHARACTER_HPP
#define CHARACTER_HPP


#include "glm/vec3.hpp"
#include "../camera/camera.hpp"

class Character {
public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera camera;
};

#endif //CHARACTER_HPP
