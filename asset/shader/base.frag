#version 460 core

in vec2 out_texposition;
in float out_facekind;
out vec4 color;

uniform sampler2DArray textu;
uniform int textid;

void main()
{
    int id = textid;
    if(id == 3){ // grass
        if(out_facekind == 1){ // top 
            id = 0;
        }
        if(out_facekind == -1){ // bottom
            id = 2; 
        }

    }
    color = texture(textu, vec3(out_texposition, int(id)));
}
