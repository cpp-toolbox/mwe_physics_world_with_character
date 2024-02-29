#version 330

attribute vec3 vPos;
attribute vec3 vNorm;
attribute vec2 vTexPos;

uniform mat4 local_to_world;
uniform mat4 world_to_camera;
uniform mat4 camera_to_clip;

out vec2 texPos;

void main() {
    gl_Position = camera_to_clip * world_to_camera * local_to_world * vec4(vPos, 1.0);
    texPos = vTexPos;
}
