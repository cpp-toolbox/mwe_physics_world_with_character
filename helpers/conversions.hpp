#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include "glm/glm.hpp"
#include "Jolt/Jolt.h"

JPH::Vec3 convert_vec3_from_glm_to_jolt(glm::vec3 input_vector);
glm::vec3 convert_vec3_from_jolt_to_glm(JPH::Vec3 input_vector);
float angle_between( glm::vec3 a, glm::vec3 b, glm::vec3 origin );

#endif
