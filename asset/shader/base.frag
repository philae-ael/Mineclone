#version 460 core

in vec3 out_barposition;
out vec4 color;

void main()
{
    color = vec4(out_barposition, 0.0);
}
