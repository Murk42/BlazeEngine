#version 450    

layout(location = 0) in vec2 frag_pos;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in vec4 frag_color;

layout(location = 0) out vec4 outColor;

void main() 
{    
    outColor = vec4(0, 1, 1, 1.0);
}