#include "conversions.hpp"

#include <cmath>

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

glm::mat4 convert_mat4_from_jolt_to_glm(JPH::Mat44 input_matrix) {
    glm::mat4 glm_mat;
    for (int col = 0; col < 4; col ++) {
        for (int row = 0; row < 4; row ++) {
            glm_mat[col][row] = input_matrix(row, col);
        }
    }
    return glm_mat;
}


JPH::Array<JPH::Vec3> fibonacci_sphere(int num_samples) {
    JPH::Array<JPH::Vec3> points;
    float phi = M_PI * (std::sqrt(5.0) - 1.0);

    for (int i = 0; i < num_samples; i ++) {
        float y = 1 - ((float) i / ((float) num_samples - 1)) * 2;
        float radius = std::sqrt(1 - y * y);
        float theta = phi * (float) i;

        float x = std::cos(theta) * radius;
        float z = std::sin(theta) * radius;
        points.emplace_back(x, y, z);
    }
    return points;
}

//def fibonacci_sphere(samples=1000):
//
//points = []
//phi = math.pi * (math.sqrt(5.) - 1.)  # golden angle in radians
//
//for i in range(samples):
//y = 1 - (i / float(samples - 1)) * 2  # y goes from 1 to -1
//radius = math.sqrt(1 - y * y)  # radius at y
//
//        theta = phi * i  # golden angle increment
//
//        x = math.cos(theta) * radius
//z = math.sin(theta) * radius
//
//points.append((x, y, z))
//
//return points

