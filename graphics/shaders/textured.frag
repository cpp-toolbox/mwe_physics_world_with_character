#version 330

in vec2 texPos;
uniform sampler2D texture_sampler;
varying vec3 color;

void main() {
    gl_FragColor = texture(texture_sampler, texPos);
}
