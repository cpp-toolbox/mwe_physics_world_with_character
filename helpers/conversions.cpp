#include "conversions.hpp"

glm::vec3 standard_to_player_basis_transformation(glm::vec3 standard_vector, glm::vec3 player_forward) {
    glm::vec3 player_forward_normalized = glm::normalize(player_forward);
    glm::vec3 player_left = glm::normalize(glm::cross(player_forward, glm::vec3(0, 1, 0)));
    glm::vec player_up = glm::normalize(glm::cross(player_forward_normalized, player_left));

    glm::mat3 standard_to_player_transformation_matrix = glm::mat3(player_up, player_left, player_forward_normalized);

    return standard_to_player_transformation_matrix * standard_vector;
}


float angle_between(glm::vec3 a, glm::vec3 b, glm::vec3 origin) {
    glm::vec3 da = glm::normalize(a-origin);
    glm::vec3 db = glm::normalize(b-origin);
    return glm::acos(glm::dot(da, db));
}


glm::vec3 convert_vec3_from_jolt_to_glm(JPH::Vec3 input_vector) {
    return {input_vector.GetX(), input_vector.GetY(), input_vector.GetZ()} ;
}

JPH::Vec3 convert_vec3_from_glm_to_jolt(glm::vec3 input_vector) {
    return {input_vector.x, input_vector.y, input_vector.z} ;
}
