#version 460 core

in vec3 in_position;

uniform mat4 transform;

void main(void) {
    gl_Position = transform * vec4(in_position, 1.0);
}
