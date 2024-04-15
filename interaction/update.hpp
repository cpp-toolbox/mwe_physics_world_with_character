#ifndef UPDATE_HPP
#define UPDATE_HPP

#include "camera/camera.hpp"
#include "input_snapshot/input_snapshot.hpp"
#include "mouse/mouse.hpp"
#include "physics/physics.hpp"

void update(double time_since_last_update, Physics *physics, Camera *camera, InputSnapshot *input_snapshot,
            Mouse *mouse, float movement_acceleration);

std::function<void(double)> create_update_closure(Physics *physics, Camera *camera, InputSnapshot *input_snapshot,
                                                  Mouse *mouse, float movement_acceleration);

#endif
