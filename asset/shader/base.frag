#version 460 core

in vec2 out_texposition;
flat in int out_textid;
out vec4 color;

uniform sampler2DArray textu;

void main()
{
    color = texture(textu, vec3(out_texposition, out_textid));
}
