#version 460 core

in vec3 in_position;

void main(void) {
    gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
}
