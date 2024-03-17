#version 330

attribute vec3 position;

uniform mat4 local_to_world;
uniform mat4 world_to_camera;
uniform mat4 camera_to_clip;

void main() {
    gl_Position = camera_to_clip * world_to_camera * local_to_world * vec4(position, 1.0);
}
