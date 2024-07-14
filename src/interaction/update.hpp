#ifndef UPDATE_HPP
#define UPDATE_HPP

#include "camera/camera.hpp"
#include "mouse/mouse.hpp"
#include "physics/physics.hpp"
#include "../graphics/window/window.hpp"

void update(double time_since_last_update, Physics *physics, Camera *camera, LiveInputState *live_input_state,
            Mouse *mouse, float movement_acceleration);

std::function<void(double)> create_update_closure(Physics *physics, Camera *camera, LiveInputState *live_input_state,
                                                  Mouse *mouse, float movement_acceleration);

#endif
