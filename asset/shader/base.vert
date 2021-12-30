#version 460 core

in vec3 in_position;
in vec2 in_texposition;
in float in_facekind;
out vec2 out_texposition;
out float out_facekind;

uniform mat4 model;
uniform mat4 world;
uniform mat4 proj;

void main(void) {
    gl_Position = proj * world * model * vec4(in_position, 1.0);

    out_texposition = in_texposition;
    out_facekind = in_facekind;
}
