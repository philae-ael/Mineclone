#version 460 core

in vec3 in_position;
in vec3 in_barposition;
out vec3 out_barposition;

uniform mat4 model;
uniform mat4 world;
uniform mat4 proj;

void main(void) {
    gl_Position = proj * world * model * vec4(in_position, 1.0);

    out_barposition = in_barposition;
}
